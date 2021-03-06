// Copyright (c) 2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "views/focus/accelerator_handler.h"

#include "app/keyboard_code_conversion_win.h"
#include "app/keyboard_codes.h"
#include "base/win_util.h"
#include "views/event.h"
#include "views/focus/focus_manager.h"

namespace views {

AcceleratorHandler::AcceleratorHandler() {
}

bool AcceleratorHandler::Dispatch(const MSG& msg) {
  bool process_message = true;

  if (msg.message >= WM_KEYFIRST && msg.message <= WM_KEYLAST) {
    FocusManager* focus_manager =
        FocusManager::GetFocusManagerForNativeView(msg.hwnd);
    if (focus_manager) {
      switch (msg.message) {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN: {
          KeyEvent event(Event::ET_KEY_PRESSED,
                         app::KeyboardCodeForWindowsKeyCode(msg.wParam),
                         KeyEvent::GetKeyStateFlags(),
                         msg.lParam & 0xFFFF,
                         (msg.lParam & 0xFFFF0000) >> 16);
          process_message = focus_manager->OnKeyEvent(event);
          if (!process_message) {
            // Record that this key is pressed so we can remember not to
            // translate and dispatch the associated WM_KEYUP.
            pressed_keys_.insert(msg.wParam);
          }
          break;
        }
        case WM_KEYUP:
        case WM_SYSKEYUP: {
          std::set<WPARAM>::iterator iter = pressed_keys_.find(msg.wParam);
          if (iter != pressed_keys_.end()) {
            // Don't translate/dispatch the KEYUP since we have eaten the
            // associated KEYDOWN.
            pressed_keys_.erase(iter);
            return true;
          }
          break;
        }
      }
    }
  }

  if (process_message) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return true;
}

}  // namespace views
