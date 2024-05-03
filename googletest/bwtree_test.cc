#include "bwtree.h"
#include "bwtree_test_util.h"
#include "multithread_test_util.h"
#include "worker_pool.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <random>
#include <vector>
#include <string>

/*******************************************************************************
 * The test structures stated here were written to give you and idea of what a
 * test should contain and look like. Feel free to change the code and add new
 * tests of your own. The more concrete your tests are, the easier it'd be to
 * detect bugs in the future projects.
 ******************************************************************************/

/*
 * Tests bwtree init/destroy APIs.
 */
TEST(BwtreeInitTest, HandlesInitialization) {
  auto *const tree = test::BwTreeTestUtil::GetEmptyTree();

  ASSERT_TRUE(tree != nullptr);

  delete tree;
}

/*
 * TestFixture for bwtree tests
 */
class BwtreeTest : public ::testing::Test {
  protected:
    /*
     * NOTE: You can also use constructor/destructor instead of SetUp() and
     * TearDown(). The official document says that the former is actually
     * perferred due to some reasons. Checkout the document for the difference.
     */
    BwtreeTest() {
      num_threads_ =
        test::MultiThreadTestUtil::HardwareConcurrency() +
          (test::MultiThreadTestUtil::HardwareConcurrency() % 2);

      /* 
       * If the number of cores is too small,
       * set the number of threads large to see the problem.
       */
      if (num_threads_ < 32) {
        num_threads_ = 32;
      }
    }

    ~BwtreeTest() {
    }

    uint32_t num_threads_;

    const uint32_t read_cycle = 16;
};

/*
 * Basic functionality test of 4096 concurrent random inserts
 */
TEST_F(BwtreeTest, ConcurrentRandomInsert) {
  // This defines the key space (0 ~ 4095)
  const uint32_t key_num = 4096;
  std::atomic<size_t> insert_success_counter = 0;

  common::WorkerPool thread_pool(num_threads_, {});
  thread_pool.Startup();
  auto *const tree = test::BwTreeTestUtil::GetEmptyTree();

  // Inserts in a 1M key space randomly until all keys has been inserted
  auto workload = [&](uint32_t id) {
    const uint32_t gcid = id + 1;
    tree->AssignGCID(gcid);
    std::default_random_engine thread_generator(id);
    std::uniform_int_distribution<int> uniform_dist(0, key_num - 1);

    while (insert_success_counter.load() < key_num) {
      int key = uniform_dist(thread_generator);

      if (tree->Insert(key, key)) insert_success_counter.fetch_add(1);
    }
    tree->UnregisterThread(gcid);
  };

  tree->UpdateThreadLocal(num_threads_ + 1);
  test::MultiThreadTestUtil::RunThreadsUntilFinish(&thread_pool, num_threads_, workload);
  tree->UpdateThreadLocal(1);

  // Verifies whether random insert is correct
  for (uint32_t i = 0; i < key_num; i++) {
    auto s = tree->GetValue(i);

    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(*s.begin(), i);
  }

  delete tree;
}

/*
 * Basic functionality test of 4096 concurrent random inserts and reads.
 */
TEST_F(BwtreeTest, ConcurrentRandomInsertRead) {
  // This defines the key space (0 ~ 4095)
  const uint32_t key_num = 4096;
  std::atomic<size_t> insert_success_counter = 0;

  common::WorkerPool thread_pool(num_threads_, {});
  thread_pool.Startup();
  auto *const tree = test::BwTreeTestUtil::GetEmptyTree();

  // Inserts in a 1M key space randomly until all keys has been inserted
  auto workload = [&](uint32_t id) {
    const uint32_t gcid = id + 1;
    tree->AssignGCID(gcid);
    std::default_random_engine thread_generator(id);
    std::uniform_int_distribution<int> uniform_dist(0, key_num - 1);
    std::vector<int> key_vector;

    while (insert_success_counter.load() < key_num) {
      int key = uniform_dist(thread_generator);

      if (tree->Insert(key, key)) {
        insert_success_counter.fetch_add(1);

        key_vector.push_back(key);
        if (key_vector.size() == read_cycle) {
          std::vector<int>::iterator iter;
          for (iter = key_vector.begin(); iter != key_vector.end(); iter++) {
            auto s = tree->GetValue(*iter);
            EXPECT_NE(s.size(), 0);
          }
          key_vector.clear();
        }
      }
    }
    tree->UnregisterThread(gcid);
  };

  tree->UpdateThreadLocal(num_threads_ + 1);
  test::MultiThreadTestUtil::RunThreadsUntilFinish(&thread_pool, num_threads_, workload);
  tree->UpdateThreadLocal(1);

  // Verifies whether random insert is correct
  for (uint32_t i = 0; i < key_num; i++) {
    auto s = tree->GetValue(i);

    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(*s.begin(), i);
  }

  delete tree;
}

/*
 * Basic functionality test of 4096 concurrent random inserts and deletes.
 */
TEST_F(BwtreeTest, ConcurrentRandomInsertDelete) {
  NOISEPAGE_ASSERT(num_threads_ % 2 == 0,
      "This test requires an even number of threads. This should have been handled when it was assigned.");

  // This defines the key space (0 ~ 4095)
  const uint32_t key_num = 4096;

  common::WorkerPool thread_pool(num_threads_, {});
  thread_pool.Startup();
  auto *const tree = test::BwTreeTestUtil::GetEmptyTree();

  auto workload = [&](uint32_t id) {
    const uint32_t gcid = id + 1;
    tree->AssignGCID(gcid);
    if ((id % 2) == 0) {
      for (uint32_t i = 0; i < key_num; i++) {
        int key = num_threads_ * i + id;  // NOLINT

        tree->Insert(key, key);
      }
    } else {
      for (uint32_t i = 0; i < key_num; i++) {
        int key = num_threads_ * i + id - 1;  // NOLINT

        while (!tree->Delete(key, key)) {
        }
      }
    }
    tree->UnregisterThread(gcid);
  };

  tree->UpdateThreadLocal(num_threads_ + 1);
  test::MultiThreadTestUtil::RunThreadsUntilFinish(&thread_pool, num_threads_, workload);
  tree->UpdateThreadLocal(1);

  // Verifies that all values are deleted after mixed test
  for (uint32_t i = 0; i < key_num * num_threads_; i++) {
    EXPECT_EQ(tree->GetValue(i).size(), 0);
  }

  delete tree;
}

/*
 * Basic functionality test of 4096 concurrent random inserts, deletes and reads.
 */
TEST_F(BwtreeTest, ConcurrentRandomInsertDeleteRead) {
  NOISEPAGE_ASSERT(num_threads_ % 2 == 0,
      "This test requires an even number of threads. This should have been handled when it was assigned.");

  // This defines the key space (0 ~ 4095)
  const uint32_t key_num = 4096;

  common::WorkerPool thread_pool(num_threads_, {});
  thread_pool.Startup();
  auto *const tree = test::BwTreeTestUtil::GetEmptyTree();

  auto workload = [&](uint32_t id) {
    const uint32_t gcid = id + 1;
    tree->AssignGCID(gcid);

    if ((id % 2) == 0) {
      int prev_counter = 0;
      std::vector<int> key_vector;

      for (uint32_t i = 0; i < key_num; i++) {
        int key = num_threads_ * i + id;  // NOLINT

        if (tree->Insert(key, key)) {
          key_vector.push_back(key);
          if (key_vector.size() == read_cycle) {
            std::vector<int>::iterator iter;
            for (iter = key_vector.begin(); iter != key_vector.end(); iter++) {
              auto s = tree->GetValue(*iter);
            }
            key_vector.clear();
          }
        }
      }
    } else {
      for (uint32_t i = 0; i < key_num; i++) {
        int key = num_threads_ * i + id - 1;  // NOLINT

        while (!tree->Delete(key, key)) {
        }
      }
    }
    tree->UnregisterThread(gcid);
  };

  tree->UpdateThreadLocal(num_threads_ + 1);
  test::MultiThreadTestUtil::RunThreadsUntilFinish(&thread_pool, num_threads_, workload);
  tree->UpdateThreadLocal(1);

  // Verifies that all values are deleted after mixed test
  for (uint32_t i = 0; i < key_num * num_threads_; i++) {
    EXPECT_EQ(tree->GetValue(i).size(), 0);
  }

  delete tree;
}

/*
 * Basic functionality test of 4096 concurrent skewed inserts
 */
TEST_F(BwtreeTest, ConcurrentSkewedInsert) {
  // This defines the key space (0 ~ 4095)
  const uint32_t key_num = 4096;
  std::atomic<size_t> insert_success_counter = 0;
  int key = 0xABCD;

  common::WorkerPool thread_pool(num_threads_, {});
  thread_pool.Startup();
  auto *const tree = test::BwTreeTestUtil::GetEmptyTree();

  // Inserts in a 1M key space randomly until all keys has been inserted
  auto workload = [&](uint32_t id) {
    const uint32_t gcid = id + 1;
    tree->AssignGCID(gcid);
    std::default_random_engine thread_generator(id);
    std::uniform_int_distribution<int> uniform_dist(0, key_num - 1);

    while (insert_success_counter.load() < key_num) {
      int value = uniform_dist(thread_generator);

      if (tree->Insert(key, value)) insert_success_counter.fetch_add(1);
    }
    tree->UnregisterThread(gcid);
  };

  tree->UpdateThreadLocal(num_threads_ + 1);
  test::MultiThreadTestUtil::RunThreadsUntilFinish(&thread_pool, num_threads_, workload);
  tree->UpdateThreadLocal(1);

  delete tree;
}

/*
 * Basic functionality test of 4096 concurrent skewed inserts and reads.
 */
TEST_F(BwtreeTest, ConcurrentSkewedInsertRead) {
  // This defines the key space (0 ~ 4095)
  const uint32_t key_num = 4096;
  std::atomic<size_t> insert_success_counter = 0;
  int key = 0xABCD;

  common::WorkerPool thread_pool(num_threads_, {});
  thread_pool.Startup();
  auto *const tree = test::BwTreeTestUtil::GetEmptyTree();

  // Inserts in a 1M key space randomly until all keys has been inserted
  auto workload = [&](uint32_t id) {
    const uint32_t gcid = id + 1;
    tree->AssignGCID(gcid);
    std::default_random_engine thread_generator(id);
    std::uniform_int_distribution<int> uniform_dist(0, key_num - 1);
    std::vector<int> key_vector;

    while (insert_success_counter.load() < key_num) {
      int value = uniform_dist(thread_generator);

      if (tree->Insert(key, value)) {
        insert_success_counter.fetch_add(1);

        key_vector.push_back(key);
        if (key_vector.size() == read_cycle) {
          std::vector<int>::iterator iter;
          for (iter = key_vector.begin(); iter != key_vector.end(); iter++) {
            auto s = tree->GetValue(*iter);
          }
          key_vector.clear();
        }
      }
    }
    tree->UnregisterThread(gcid);
  };

  tree->UpdateThreadLocal(num_threads_ + 1);
  test::MultiThreadTestUtil::RunThreadsUntilFinish(&thread_pool, num_threads_, workload);
  tree->UpdateThreadLocal(1);

  delete tree;
}

/*
 * Basic functionality test of 4096 concurrent skewed inserts and deletes.
 */
TEST_F(BwtreeTest, ConcurrentSkewedInsertDelete) {
  NOISEPAGE_ASSERT(num_threads_ % 2 == 0,
      "This test requires an even number of threads. This should have been handled when it was assigned.");

  // This defines the key space (0 ~ 4095)
  const uint32_t key_num = 4096;
  int key = 0xABCD;

  common::WorkerPool thread_pool(num_threads_, {});
  thread_pool.Startup();
  auto *const tree = test::BwTreeTestUtil::GetEmptyTree();

  // Inserts in a 1M key space randomly until all keys has been inserted
  auto workload = [&](uint32_t id) {
    const uint32_t gcid = id + 1;
    tree->AssignGCID(gcid);

    if ((id % 2) == 0) {
      for (uint32_t i = 0; i < key_num; i++) {
        int value = num_threads_ * i + id;
        tree->Insert(key, value);
      }
    } else {
      for (uint32_t i = 0; i < key_num; i++) {
        int value = num_threads_ * i + id - 1;  // NOLINT
        while (!tree->Delete(key, value)) {
        }
      }
    }

    tree->UnregisterThread(gcid);
  };

  tree->UpdateThreadLocal(num_threads_ + 1);
  test::MultiThreadTestUtil::RunThreadsUntilFinish(&thread_pool, num_threads_, workload);
  tree->UpdateThreadLocal(1);

  EXPECT_EQ(tree->GetValue(key).size(), 0);

  delete tree;
}

/*
 * Basic functionality test of 4096 concurrent skewed inserts, deletes and reads.
 */
TEST_F(BwtreeTest, ConcurrentSkewedInsertDeleteRead) {
  NOISEPAGE_ASSERT(num_threads_ % 2 == 0,
      "This test requires an even number of threads. This should have been handled when it was assigned.");

  // This defines the key space (0 ~ 4095)
  const uint32_t key_num = 4096;
  int key = 0xABCD;

  common::WorkerPool thread_pool(num_threads_, {});
  thread_pool.Startup();
  auto *const tree = test::BwTreeTestUtil::GetEmptyTree();

  auto workload = [&](uint32_t id) {
    const uint32_t gcid = id + 1;
    tree->AssignGCID(gcid);

    if ((id % 2) == 0) {
      std::vector<int> key_vector;

      for (uint32_t i = 0; i < key_num; i++) {
        int value = num_threads_ * i + id;  // NOLINT

        if (tree->Insert(key, value)) {
          key_vector.push_back(key);

          if (key_vector.size() == read_cycle) {
            std::vector<int>::iterator iter;
            for (iter = key_vector.begin(); iter != key_vector.end(); iter++) {
              auto s = tree->GetValue(*iter);
            }
            key_vector.clear();
          }
        }
      }
    } else {
      for (uint32_t i = 0; i < key_num; i++) {
        int value = num_threads_ * i + id - 1;  // NOLINT

        while (!tree->Delete(key, value)) {
        }
      }
    }
    tree->UnregisterThread(gcid);
  };

  tree->UpdateThreadLocal(num_threads_ + 1);
  test::MultiThreadTestUtil::RunThreadsUntilFinish(&thread_pool, num_threads_, workload);
  tree->UpdateThreadLocal(1);

  EXPECT_EQ(tree->GetValue(key).size(), 0);

  delete tree;
}
