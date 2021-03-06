// Copyright (c) 2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/mach_broker_mac.h"

#include "base/lock.h"
#include "testing/gtest/include/gtest/gtest.h"

class MachBrokerTest : public testing::Test {
 public:
  // Helper function to acquire/release locks and call |PlaceholderForPid()|.
  void AddPlaceholderForPid(base::ProcessHandle pid) {
    AutoLock lock(broker_.GetLock());
    broker_.AddPlaceholderForPid(pid);
  }

  // Helper function to acquire/release locks and call |FinalizePid()|.
  void FinalizePid(base::ProcessHandle pid,
                   const MachBroker::MachInfo& mach_info) {
    AutoLock lock(broker_.GetLock());
    broker_.FinalizePid(pid, mach_info);
  }

 protected:
  MachBroker broker_;
};

TEST_F(MachBrokerTest, Locks) {
  // Acquire and release the locks.  Nothing bad should happen.
  AutoLock lock(broker_.GetLock());
}

TEST_F(MachBrokerTest, AddPlaceholderAndFinalize) {
  // Add a placeholder for PID 1.
  AddPlaceholderForPid(1);
  EXPECT_EQ(0u, broker_.TaskForPid(1));

  // Finalize PID 1.
  FinalizePid(1, MachBroker::MachInfo().SetTask(100u));
  EXPECT_EQ(100u, broker_.TaskForPid(1));

  // Should be no entry for PID 2.
  EXPECT_EQ(0u, broker_.TaskForPid(2));
}

TEST_F(MachBrokerTest, Invalidate) {
  AddPlaceholderForPid(1);
  FinalizePid(1, MachBroker::MachInfo().SetTask(100u));

  EXPECT_EQ(100u, broker_.TaskForPid(1));
  broker_.InvalidatePid(1u);
  EXPECT_EQ(0u, broker_.TaskForPid(1));
}

TEST_F(MachBrokerTest, FinalizeUnknownPid) {
  // Finalizing an entry for an unknown pid should not add it to the map.
  FinalizePid(1u, MachBroker::MachInfo().SetTask(100u));
  EXPECT_EQ(0u, broker_.TaskForPid(1u));
}
