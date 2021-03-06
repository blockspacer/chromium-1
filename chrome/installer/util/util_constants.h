// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Defines all install related constants that need to be used by Chrome as
// well as Chrome Installer.

#ifndef CHROME_INSTALLER_UTIL_UTIL_CONSTANTS_H_
#define CHROME_INSTALLER_UTIL_UTIL_CONSTANTS_H_
#pragma once

namespace installer {

// Return status of installer
enum InstallStatus {
  FIRST_INSTALL_SUCCESS, // Successfully installed Chrome for the first time
  INSTALL_REPAIRED,      // Same version reinstalled for repair
  NEW_VERSION_UPDATED,   // Chrome successfully updated to new version
  EXISTING_VERSION_LAUNCHED,  // No work done, just launched existing chrome
  HIGHER_VERSION_EXISTS, // Higher version of Chrome already exists
  USER_LEVEL_INSTALL_EXISTS, // User level install already exists
  SYSTEM_LEVEL_INSTALL_EXISTS, // Machine level install already exists
  INSTALL_FAILED,        // Install/update failed
  SETUP_PATCH_FAILED,    // Failed to patch setup.exe
  OS_NOT_SUPPORTED,      // Current OS not supported
  OS_ERROR,              // OS API call failed
  TEMP_DIR_FAILED,       // Unable to get Temp directory
  UNCOMPRESSION_FAILED,  // Failed to uncompress Chrome archive
  INVALID_ARCHIVE,       // Something wrong with the installer archive
  INSUFFICIENT_RIGHTS,   // User trying system level install is not Admin
  CHROME_NOT_INSTALLED,  // Chrome not installed (returned in case of uninstall)
  CHROME_RUNNING,        // Chrome currently running (when trying to uninstall)
  UNINSTALL_CONFIRMED,   // User has confirmed Chrome uninstall
  UNINSTALL_DELETE_PROFILE, // User confirmed uninstall and profile deletion
  UNINSTALL_SUCCESSFUL,  // Chrome successfully uninstalled
  UNINSTALL_FAILED,      // Chrome uninstallation failed
  UNINSTALL_CANCELLED,   // User cancelled Chrome uninstallation
  UNKNOWN_STATUS,        // Unknown status (this should never happen)
  RENAME_SUCCESSFUL,     // Rename of new_chrome.exe to chrome.exe worked
  RENAME_FAILED,         // Rename of new_chrome.exe failed
  EULA_REJECTED,         // EULA dialog was not accepted by user.
  EULA_ACCEPTED,         // EULA dialog was accepted by user.
  EULA_ACCEPTED_OPT_IN,  // EULA accepted wtih the crash optin selected.
  INSTALL_DIR_IN_USE,    // Installation directory is in use by another process
  UNINSTALL_REQUIRES_REBOOT, // Uninstallation required a reboot.
  IN_USE_UPDATED,        // Chrome successfully updated but old version running
  SAME_VERSION_REPAIR_FAILED, // Chrome repair failed as Chrome was running
  REENTRY_SYS_UPDATE,    // Setup has been re-launched as the interactive user
  SXS_OPTION_NOT_SUPPORTED  // The chrome-sxs option provided does not work
                            // with other command line options.
};

namespace switches {
extern const char kCeee[];
extern const char kChrome[];
extern const char kChromeFrame[];
extern const char kChromeSxS[];
extern const char kCreateAllShortcuts[];
extern const char kDeleteProfile[];
extern const char kDisableLogging[];
extern const char kDoNotCreateShortcuts[];
extern const char kDoNotLaunchChrome[];
extern const char kDoNotRegisterForUpdateLaunch[];
extern const char kDoNotRemoveSharedItems[];
extern const char kEnableLogging[];
extern const char kForceUninstall[];
extern const char kInstallArchive[];
extern const char kInstallerData[];
extern const char kLogFile[];
extern const char kMakeChromeDefault[];
extern const char kMsi[];
extern const char kMultiInstall[];
extern const char kNewSetupExe[];
extern const char kRegisterChromeBrowser[];
extern const char kRegisterChromeBrowserSuffix[];
extern const char kRenameChromeExe[];
extern const char kRemoveChromeRegistration[];
extern const char kRunAsAdmin[];
extern const char kSystemLevel[];
extern const char kUninstall[];
extern const char kUpdateSetupExe[];
extern const char kVerboseLogging[];
extern const char kShowEula[];
extern const char kAltDesktopShortcut[];
extern const char kInactiveUserToast[];
extern const char kSystemLevelToast[];
extern const char kToastResultsKey[];
}  // namespace switches

extern const wchar_t kCeeeBrokerExe[];
extern const wchar_t kCeeeIeDll[];
extern const wchar_t kCeeeInstallHelperDll[];
extern const wchar_t kChromeDll[];
extern const wchar_t kChromeExe[];
extern const wchar_t kChromeFrameDll[];
extern const wchar_t kChromeFrameHelperExe[];
extern const wchar_t kChromeFrameHelperWndClass[];
extern const wchar_t kChromeNaCl64Dll[];
extern const wchar_t kChromeOldExe[];
extern const wchar_t kChromeNewExe[];
extern const wchar_t kGoogleChromeInstallSubDir1[];
extern const wchar_t kGoogleChromeInstallSubDir2[];
extern const wchar_t kInstallBinaryDir[];
extern const wchar_t kInstallerDir[];
extern const wchar_t kInstallUserDataDir[];
extern const wchar_t kNaClExe[];
extern const wchar_t kSetupExe[];
extern const wchar_t kSxSSuffix[];
extern const wchar_t kUninstallArgumentsField[];
extern const wchar_t kUninstallDisplayNameField[];
extern const wchar_t kUninstallInstallationDate[];
extern const char kUninstallMetricsName[];
extern const wchar_t kUninstallStringField[];

// Used by ProductInstall::WriteInstallerResult.
extern const wchar_t kInstallerResult[];
extern const wchar_t kInstallerError[];
extern const wchar_t kInstallerResultUIString[];
extern const wchar_t kInstallerSuccessLaunchCmdLine[];

}  // namespace installer

#endif  // CHROME_INSTALLER_UTIL_UTIL_CONSTANTS_H_
