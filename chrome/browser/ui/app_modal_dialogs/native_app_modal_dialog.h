// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_APP_MODAL_DIALOGS_NATIVE_APP_MODAL_DIALOG_H_
#define CHROME_BROWSER_UI_APP_MODAL_DIALOGS_NATIVE_APP_MODAL_DIALOG_H_
#pragma once

#include "gfx/native_widget_types.h"

class JavaScriptAppModalDialog;

class NativeAppModalDialog {
 public:
  // Returns the buttons to be shown. See MessageBoxFlags for a description of
  // the values.
  virtual int GetAppModalDialogButtons() const = 0;

  // Shows the dialog.
  virtual void ShowAppModalDialog() = 0;

  // Activates the dialog.
  virtual void ActivateAppModalDialog() = 0;

  // Closes the dialog.
  virtual void CloseAppModalDialog() = 0;

  // Accepts or cancels the dialog.
  virtual void AcceptAppModalDialog() = 0;
  virtual void CancelAppModalDialog() = 0;

  // Creates an app modal dialog for a JavaScript prompt.
  static NativeAppModalDialog* CreateNativeJavaScriptPrompt(
      JavaScriptAppModalDialog* dialog,
      gfx::NativeWindow parent_window);
};

#endif  // CHROME_BROWSER_UI_APP_MODAL_DIALOGS_NATIVE_APP_MODAL_DIALOG_H_

