// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_COMMON_PREF_STORE_OBSERVER_MOCK_H_
#define CHROME_COMMON_PREF_STORE_OBSERVER_MOCK_H_
#pragma once

#include "base/basictypes.h"
#include "chrome/common/pref_store.h"
#include "testing/gmock/include/gmock/gmock.h"

// A gmock-ified implementation of PrefStore::ObserverInterface.
class PrefStoreObserverMock : public PrefStore::ObserverInterface {
 public:
  PrefStoreObserverMock() {}
  virtual ~PrefStoreObserverMock() {}

  MOCK_METHOD1(OnPrefValueChanged, void(const std::string&));
  MOCK_METHOD0(OnInitializationCompleted, void());

 private:
  DISALLOW_COPY_AND_ASSIGN(PrefStoreObserverMock);
};

#endif  // CHROME_COMMON_PREF_STORE_OBSERVER_MOCK_H_
