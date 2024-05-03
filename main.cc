#include <algorithm>
#include <random>
#include <vector>
#include <string>
#include <iostream>

#include "bwtree.h"
#include "bwtree_test_util.h"
#include "multithread_test_util.h"
#include "timer.h"
#include "worker_pool.h"
#include "zipf.h"

/*
 * This workload is the ConcurrentSkewedInsertDeleteRead test in
 * googletest/bwtree_test.cc
 *
 * We set the number of workers to the number of logical processors available
 * to run threads on the user's computer. Since the workload requires pairs of
 * workers, set the number to be even.
 *
 * Two worker threads form a pair. There will probably be multiple such pairs.
 * One worker inserts records and periodically scans for the keys it inserted.
 * Another worker deletes the keys inserted by that worker.
 * 
 * In other words, two workers sharing a queue, one worker filling the queue,
 * and the other clearing the queue.
 *
 * Unlike the ConcurrentRandomInsertDeleteRead test in bwtree_test.cc,
 * all records have the same key, making skewed situation.
 *
 * You can breakdown this workload and figure out the problems. You can use the
 * timer library provided by c++ standard, or you can use the function of
 * timer.h in the include directory. Of course, you can create your own
 * functions for anlayzing.
 */
int main(int argc, char *argv[]) {
  const uint32_t num_threads_ =
    test::MultiThreadTestUtil::HardwareConcurrency() + (test::MultiThreadTestUtil::HardwareConcurrency() % 2);
  const uint32_t key_num = 4096;
  int key = 0xABCD, read_cycle = 16;

  /*
   * Initialize thread pool and bwtree.
   */
  common::WorkerPool thread_pool(num_threads_, {});
  thread_pool.Startup();
  auto *const tree = test::BwTreeTestUtil::GetEmptyTree();

  /*
   * Function to be performed by each worker.
   * See the ConcurrentSkewedInsertDeleteRead test case in googletest/bwtree_test.cc
   */
  auto workload = [&](uint32_t id) {
    const uint32_t gcid = id + 1;
    tree->AssignGCID(gcid);

    /*
     * Even-numbered workers insert records and read periodically.
     */ 
    if ((id % 2) == 0) {
      std::vector<int> key_vector;

      for (uint32_t i = 0; i < key_num; i++) {
        int value = num_threads_ * i + id;

        if (tree->Insert(key, value)) {
          key_vector.push_back(key);

          /* Periodically scans the records inserted by this worker */
          if (key_vector.size() == read_cycle) {
            std::vector<int>::iterator iter;
            for (iter = key_vector.begin(); iter != key_vector.end(); iter++) {
              auto s = tree->GetValue(*iter);
            }
            key_vector.clear();
          }
        }
      }
    } else { /* Odd-numbered workers delete records */
      for (uint32_t i = 0; i < key_num; i++) {
        /* The value inserted by the above worker */
        int value = num_threads_ * i + id - 1;
        while (!tree->Delete(key, value)) {
        }
      }
    }
    tree->UnregisterThread(gcid);
  };

  tree->UpdateThreadLocal(num_threads_ + 1);
  test::MultiThreadTestUtil::RunThreadsUntilFinish(&thread_pool, num_threads_, workload);
  tree->UpdateThreadLocal(1);

  delete tree;

  return 0;
}
