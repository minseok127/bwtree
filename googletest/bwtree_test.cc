#include "bwtree.h"
#include "bwtree_test_util.h"
#include "multithread_test_util.h"
#include "worker_pool.h"

#include <climits>
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
class BwtreeUniformTest : public ::testing::Test {
  protected:
    /*
     * NOTE: You can also use constructor/destructor instead of SetUp() and
     * TearDown(). The official document says that the former is actually
     * perferred due to some reasons. Checkout the document for the difference.
     */
    BwtreeUniformTest() {
    }

    ~BwtreeUniformTest() {
      delete tree;
    }

    const uint32_t num_threads_ = test::MultiThreadTestUtil::HardwareConcurrency();

    test::BwTreeTestUtil::TreeType *const tree = test::BwTreeTestUtil::GetEmptyTree();

    std::atomic<int> gc_id = 0;

    const uint32_t threshold = 1;
};

/*
 * Build Bwtree with 1M records.
 */
TEST_F(BwtreeUniformTest, PopulateBwTree) {
  // This defines the key space (0 ~ (1M - 1))
  const uint32_t key_num = 1024 * 1024;
  std::default_random_engine thread_generator(0);
  std::uniform_int_distribution<int> uniform_dist(0, key_num - 1);
  int thread_gc_id = gc_id.fetch_add(1);
  
  tree->AssignGCID(thread_gc_id);

  for (int i = 0; i < key_num; i++) {
    EXPECT_TRUE(tree->Insert(i, i));
  }

  tree->UnregisterThread(thread_gc_id);
}

/*
 * Basic functionality test of 1M concurrent uniform insert/delete.
 */
TEST_F(BwtreeUniformTest, ConcurrentInsertDelete) {
  // This defines the key space (0 ~ (1M - 1))
  const uint32_t key_num = 1024 * 1024;
  std::atomic<size_t> insert_success_counter = 0;

  common::WorkerPool thread_pool(num_threads_, {});
  thread_pool.Startup();

  auto workload = [&](uint32_t id) {
    const uint32_t gcid = gc_id.fetch_add(1);
    std::default_random_engine thread_generator(id);
    std::uniform_int_distribution<int> uniform_dist(0, key_num - 1);
    std::vector<std::pair<int, int>> key_value_vector;

    tree->AssignGCID(gcid);

    while (insert_success_counter.load() < key_num) {
      int key = uniform_dist(thread_generator);
      int value = uniform_dist(thread_generator);

      if (tree->Insert(key, value)) {
        insert_success_counter.fetch_add(1);
        key_value_vector.push_back(std::make_pair(key, value));
      } 
      
      if (key_value_vector.size() == threshold) {
        for (int i = 0; i < key_value_vector.size(); i++) {
          std::pair<int, int> del_pair = key_value_vector[i];

          EXPECT_TRUE(tree->Delete(del_pair.first, del_pair.second));
        }

        key_value_vector.clear();
      }
    }

    tree->UnregisterThread(gcid);
  };

  tree->UpdateThreadLocal(num_threads_ + 1);
  test::MultiThreadTestUtil::RunThreadsUntilFinish(&thread_pool, num_threads_, workload);
  tree->UpdateThreadLocal(1);
}

/*
 * TestFixture for bwtree tests
 */
class BwtreeSkewedTest : public ::testing::Test {
  protected:
    /*
     * NOTE: You can also use constructor/destructor instead of SetUp() and
     * TearDown(). The official document says that the former is actually
     * perferred due to some reasons. Checkout the document for the difference.
     */
    BwtreeSkewedTest() {
    }

    ~BwtreeSkewedTest() {
      delete tree;
    }

    const uint32_t num_threads_ = test::MultiThreadTestUtil::HardwareConcurrency();

    test::BwTreeTestUtil::TreeType *const tree = test::BwTreeTestUtil::GetEmptyTree();

    std::atomic<int> gc_id = 0;

    const uint32_t threshold = 1;
};

/*
 * Build Bwtree with 1M records.
 */
TEST_F(BwtreeSkewedTest, PopulateBwTree) {
  // This defines the key space (0 ~ (1M - 1))
  const uint32_t key_num = 1024 * 1024;
  std::default_random_engine thread_generator(0);
  std::uniform_int_distribution<int> uniform_dist(0, key_num - 1);
  int thread_gc_id = gc_id.fetch_add(1);
  
  tree->AssignGCID(thread_gc_id);

  for (int i = 0; i < key_num; i++) {
    EXPECT_TRUE(tree->Insert(i, i));
  }

  tree->UnregisterThread(thread_gc_id);
}

/*
 * Basic functionality test of 1M concurrent uniform insert/delete.
 */
TEST_F(BwtreeSkewedTest, ConcurrentInsertDelete) {
  // This defines the key space (0 ~ (1M - 1))
  const uint32_t key_num = 1024 * 1024;
  std::atomic<size_t> insert_success_counter = 0;
  int key = 0xABCD;

  common::WorkerPool thread_pool(num_threads_, {});
  thread_pool.Startup();

  auto workload = [&](uint32_t id) {
    const uint32_t gcid = gc_id.fetch_add(1);
    std::vector<std::pair<int, int>> key_value_vector;
    int value = id * key;

    tree->AssignGCID(gcid);

    while (insert_success_counter.load() < key_num) {
      if (tree->Insert(key, value)) {
        insert_success_counter.fetch_add(1);
        key_value_vector.push_back(std::make_pair(key, value));
        value++;
      } 
      
      if (key_value_vector.size() == threshold) {
        for (int i = 0; i < key_value_vector.size(); i++) {
          std::pair<int, int> del_pair = key_value_vector[i];

          EXPECT_TRUE(tree->Delete(del_pair.first, del_pair.second));
        }

        key_value_vector.clear();
        value = id * key;
      }
    }

    tree->UnregisterThread(gcid);
  };

  tree->UpdateThreadLocal(num_threads_ + 1);
  test::MultiThreadTestUtil::RunThreadsUntilFinish(&thread_pool, num_threads_, workload);
  tree->UpdateThreadLocal(1);
}

