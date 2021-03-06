// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// This class implements a simplified and much stripped down version of
// Chrome's app/resource_bundle machinery. It notably avoids unwanted
// dependencies on things like Skia.
//

#ifndef CHROME_FRAME_SIMPLE_RESOURCE_LOADER_H_
#define CHROME_FRAME_SIMPLE_RESOURCE_LOADER_H_

#include <windows.h>
#include <string>
#include <vector>

#include "base/file_path.h"
#include "base/gtest_prod_util.h"
#include "base/singleton.h"

class SimpleResourceLoader {
 public:

  static SimpleResourceLoader* GetInstance();

  // Returns the language tag for the active language.
  static std::wstring GetLanguage();

  // Helper method to return the string resource identified by message_id
  // from the currently loaded locale dll.
  static std::wstring Get(int message_id);

  // Retrieves the HINSTANCE of the loaded module handle. May be NULL if a
  // resource DLL could not be loaded.
  HMODULE GetResourceModuleHandle();

  // Retrieves the preferred languages for the current thread, adding them to
  // |language_tags|.
  static void GetPreferredLanguages(std::vector<std::wstring>* language_tags);

  // Populates |locales_path| with the path to the "Locales" directory.
  static void DetermineLocalesDirectory(FilePath* locales_path);

  // Returns false if |language_tag| is malformed.
  static bool IsValidLanguageTag(const std::wstring& language_tag);

 private:
  SimpleResourceLoader();
  ~SimpleResourceLoader();

  // Finds the most-preferred resource DLL for the laguages in |language_tags|
  // in |locales_path|.
  //
  // Returns true on success with a handle to the DLL that was loaded in
  // |dll_handle| and its path in |file_path|.
  static bool LoadLocaleDll(const std::vector<std::wstring>& language_tags,
                            const FilePath& locales_path,
                            HMODULE* dll_handle,
                            FilePath* file_path);

  // Returns the string resource identified by message_id from the currently
  // loaded locale dll.
  std::wstring GetLocalizedResource(int message_id);

  friend struct DefaultSingletonTraits<SimpleResourceLoader>;

  FRIEND_TEST_ALL_PREFIXES(SimpleResourceLoaderTest, LoadLocaleDll);

  std::wstring language_;
  HINSTANCE locale_dll_handle_;
};

#endif  // CHROME_FRAME_SIMPLE_RESOURCE_LOADER_H_
