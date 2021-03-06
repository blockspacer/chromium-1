// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef APP_MENUS_ACCELERATOR_H_
#define APP_MENUS_ACCELERATOR_H_
#pragma once

#include "app/keyboard_codes.h"

namespace menus {

// This is a cross-platform base class for accelerator keys used in menus. It is
// meant to be subclassed for concrete toolkit implementations.

class Accelerator {
 public:
  Accelerator() : key_code_(app::VKEY_UNKNOWN), modifiers_(0) {}

  Accelerator(app::KeyboardCode keycode, int modifiers)
      : key_code_(keycode),
        modifiers_(modifiers) {}

  Accelerator(const Accelerator& accelerator) {
    key_code_ = accelerator.key_code_;
    modifiers_ = accelerator.modifiers_;
  }

  virtual ~Accelerator() {}

  Accelerator& operator=(const Accelerator& accelerator) {
    if (this != &accelerator) {
      key_code_ = accelerator.key_code_;
      modifiers_ = accelerator.modifiers_;
    }
    return *this;
  }

  // We define the < operator so that the KeyboardShortcut can be used as a key
  // in a std::map.
  bool operator <(const Accelerator& rhs) const {
    if (key_code_ != rhs.key_code_)
      return key_code_ < rhs.key_code_;
    return modifiers_ < rhs.modifiers_;
  }

  bool operator ==(const Accelerator& rhs) const {
    return (key_code_ == rhs.key_code_) && (modifiers_ == rhs.modifiers_);
  }

  bool operator !=(const Accelerator& rhs) const {
    return !(*this == rhs);
  }

  app::KeyboardCode GetKeyCode() const {
    return key_code_;
  }

  int modifiers() const {
    return modifiers_;
  }

 protected:
  // The keycode (VK_...).
  app::KeyboardCode key_code_;

  // The state of the Shift/Ctrl/Alt keys (platform-dependent).
  int modifiers_;
};

// Since acclerator code is one of the few things that can't be cross platform
// in the chrome UI, separate out just the GetAcceleratorForCommandId() from
// the menu delegates.
class AcceleratorProvider {
 public:
  // Gets the accelerator for the specified command id. Returns true if the
  // command id has a valid accelerator, false otherwise.
  virtual bool GetAcceleratorForCommandId(
      int command_id,
      menus::Accelerator* accelerator) = 0;

 protected:
  virtual ~AcceleratorProvider() {}
};

}

#endif  // APP_MENUS_ACCELERATOR_H_

