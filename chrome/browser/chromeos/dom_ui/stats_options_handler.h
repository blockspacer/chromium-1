// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_CHROMEOS_DOM_UI_STATS_OPTIONS_HANDLER_H_
#define CHROME_BROWSER_CHROMEOS_DOM_UI_STATS_OPTIONS_HANDLER_H_
#pragma once

#include "chrome/browser/chromeos/dom_ui/cros_options_page_ui_handler.h"

namespace chromeos {

class MetricsCrosSettingsProvider;

// ChromeOS handler for "Stats/crash reporting to Google" option of the Advanced
// settings page. This handler does only ChromeOS-specific actions while default
// code is in Chrome's AdvancedOptionsHandler
// (chrome/browser/dom_ui/advanced_options_handler.cc).
class StatsOptionsHandler : public CrosOptionsPageUIHandler {
 public:
  StatsOptionsHandler();

  // OptionsUIHandler implementation.
  virtual void GetLocalizedValues(DictionaryValue* localized_strings);
  virtual void Initialize();

  // DOMMessageHandler implementation.
  virtual void RegisterMessages();

 private:
  MetricsCrosSettingsProvider* provider() const;
  void HandleMetricsReportingCheckbox(const ListValue* args);
  void SetupMetricsReportingCheckbox(bool user_changed);

  DISALLOW_COPY_AND_ASSIGN(StatsOptionsHandler);
};

}  // namespace chromeos

#endif  // CHROME_BROWSER_CHROMEOS_DOM_UI_STATS_OPTIONS_HANDLER_H_
