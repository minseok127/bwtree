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
 * Precheck to understand the basic operations of Bwtree.
 *
 * Insert 1M records into bwtree and delete them.
 */
int main(int argc, char *argv[]) {
  test::BwTreeTestUtil::TreeType *bwtree = test::BwTreeTestUtil::GetEmptyTree();
  const int key_num = 1024 * 1024;
  int gc_id = 0;

  bwtree->AssignGCID(gc_id);

  /*
   * 1. Insert 1M (1024*1024) records to the bwtree.
   *
   * Insert the keys from 0 to (1M - 1) sequentially into the bwtree.
   */
  

  /*
   * Check whether the insert was performed correctly.
   * "FAIL" should not be printed.
   */
  for (int key = 0; key < key_num; key++) {
    auto value_set = bwtree->GetValue(key);

    /*
     * ValueSet is std::unordered_set.
     * See the bwtree.h.
     */
    if (value_set.size() != 1) {
      std::cout << "[FAIL] Insert error (key: " << key << ")" << std::endl;
      return -1;
    }
  }
  

  /*
   * 2. Delete 1M (1024*1024) records from the bwtree.
   *
   * Delete all records.
   */
  

  /*
   * Check whether the delete was performed correctly.
   * "FAIL" should not be printed.
   */
  for (int key = 0; key < key_num; key++) {
    auto value_set = bwtree->GetValue(key);

    /*
     * ValueSet is std::unordered_set.
     * See the bwtree.h.
     */
    if (value_set.size() != 0) {
      std::cout << "[FAIL] Delete error (key: " << key << ")" << std::endl;
      return -1;
    }
  }

  bwtree->UnregisterThread(gc_id);

  delete bwtree;

  /* Success */
  std::cout << "[SUCCESS]" << std::endl;

  return 0;
}
