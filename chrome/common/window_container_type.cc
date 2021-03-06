// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/common/window_container_type.h"

#include "base/string_util.h"
#include "third_party/WebKit/WebKit/chromium/public/WebString.h"
#include "third_party/WebKit/WebKit/chromium/public/WebVector.h"
#include "third_party/WebKit/WebKit/chromium/public/WebWindowFeatures.h"

namespace {

const char kBackground[] = "background";
const char kPersistent[] = "persistent";

}

WindowContainerType WindowFeaturesToContainerType(
    const WebKit::WebWindowFeatures& window_features) {
  bool background = false;
  bool persistent = false;

  for (size_t i = 0; i < window_features.additionalFeatures.size(); ++i) {
    if (LowerCaseEqualsASCII(window_features.additionalFeatures[i],
                             kBackground))
      background = true;
    else if (LowerCaseEqualsASCII(window_features.additionalFeatures[i],
                                  kPersistent))
      persistent = true;
  }

  if (background) {
    if (persistent)
      return WINDOW_CONTAINER_TYPE_PERSISTENT;
    else
      return WINDOW_CONTAINER_TYPE_BACKGROUND;
  } else {
    return WINDOW_CONTAINER_TYPE_NORMAL;
  }
}
