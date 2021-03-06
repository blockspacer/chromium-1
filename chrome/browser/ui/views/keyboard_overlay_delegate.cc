// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/keyboard_overlay_delegate.h"

#include "app/l10n_util.h"
#include "base/scoped_ptr.h"
#include "chrome/browser/browser_list.h"
#include "chrome/browser/dom_ui/html_dialog_ui.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/common/url_constants.h"
#include "grit/generated_resources.h"


void KeyboardOverlayDelegate::ShowDialog(gfx::NativeWindow owning_window) {
  Browser* browser = BrowserList::GetLastActive();
  KeyboardOverlayDelegate* delegate = new KeyboardOverlayDelegate(
      l10n_util::GetString(IDS_KEYBOARD_OVERLAY_TITLE));
  DCHECK(browser);
  browser->BrowserShowHtmlDialog(delegate, owning_window);
}

KeyboardOverlayDelegate::KeyboardOverlayDelegate(
    const std::wstring& title)
    : title_(title) {
}

KeyboardOverlayDelegate::~KeyboardOverlayDelegate() {
}

bool KeyboardOverlayDelegate::IsDialogModal() const {
  return true;
}

std::wstring KeyboardOverlayDelegate::GetDialogTitle() const {
  return title_;
}

GURL KeyboardOverlayDelegate::GetDialogContentURL() const {
  std::string url_string(chrome::kChromeUIKeyboardOverlayURL);
  return GURL(url_string);
}

void KeyboardOverlayDelegate::GetDOMMessageHandlers(
    std::vector<DOMMessageHandler*>* handlers) const {
}

void KeyboardOverlayDelegate::GetDialogSize(
    gfx::Size* size) const {
  size->SetSize(1170, 483);
}

std::string KeyboardOverlayDelegate::GetDialogArgs() const {
  return "[]";
}

void KeyboardOverlayDelegate::OnDialogClosed(
    const std::string& json_retval) {
  delete this;
  return;
}

void KeyboardOverlayDelegate::OnCloseContents(TabContents* source,
                                              bool* out_close_dialog) {
}

bool KeyboardOverlayDelegate::ShouldShowDialogTitle() const {
  return false;
}
