// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_PREFS_PREF_VALUE_MAP_H_
#define CHROME_BROWSER_PREFS_PREF_VALUE_MAP_H_
#pragma once

#include <map>
#include <string>
#include <vector>

#include "base/basictypes.h"

class Value;

// A generic string to value map used by the PrefStore implementations.
class PrefValueMap {
 public:
  PrefValueMap();
  virtual ~PrefValueMap();

  // Gets the value for |key| and stores it in |value|. Ownership remains with
  // the map. Returns true if a value is present. If not, |value| is not
  // touched.
  bool GetValue(const std::string& key, Value** value) const;

  // Sets a new |value| for |key|. Takes ownership of |value|, which must be
  // non-NULL. Returns true if the value changed.
  bool SetValue(const std::string& key, Value* value);

  // Removes the value for |key| from the map. Returns true if a value was
  // removed.
  bool RemoveValue(const std::string& key);

  // Clears the map.
  void Clear();

  // Gets a boolean value for |key| and stores it in |value|. Returns true if
  // the value was found and of the proper type.
  bool GetBoolean(const std::string& key, bool* value) const;

  // Gets a string value for |key| and stores it in |value|. Returns true if
  // the value was found and of the proper type.
  bool GetString(const std::string& key, std::string* value) const;

  // Sets the value for |key| to the string |value|.
  void SetString(const std::string& key, const std::string& value);

  // Compares this value map against |other| and stores all key names that have
  // different values in |differing_keys|. This includes keys that are present
  // only in one of the maps.
  void GetDifferingKeys(const PrefValueMap* other,
                        std::vector<std::string>* differing_keys) const;

 private:
  typedef std::map<std::string, Value*> Map;

  Map prefs_;

  DISALLOW_COPY_AND_ASSIGN(PrefValueMap);
};

#endif  // CHROME_BROWSER_PREFS_PREF_VALUE_MAP_H_
