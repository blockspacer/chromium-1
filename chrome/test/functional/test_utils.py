#!/usr/bin/python

# Copyright (c) 2010 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import copy
import email
import os
import smtplib
import types

import pyauto_functional
import pyauto_utils


"""Commonly used functions for PyAuto tests."""

def DownloadFileFromDownloadsDataDir(test, file_name):
  """Download a file from downloads data directory, in first tab first window.

  Args:
    test: derived from pyauto.PyUITest - base class for UI test cases
    file_name: name of file to download
  """
  file_url = test.GetFileURLForDataPath(os.path.join('downloads', file_name))
  downloaded_pkg = os.path.join(test.GetDownloadDirectory().value(),
                                file_name)
  # Check if file already exists. If so then delete it.
  if os.path.exists(downloaded_pkg):
    RemoveDownloadedTestFile(test, file_name)
  test.DownloadAndWaitForStart(file_url)
  test.WaitForAllDownloadsToComplete()


def RemoveDownloadedTestFile(test, file_name):
  """Delete a file from the downloads directory.

  Arg:
    test: derived from pyauto.PyUITest - base class for UI test cases
    file_name: name of file to remove
  """
  downloaded_pkg = os.path.join(test.GetDownloadDirectory().value(),
                                file_name)
  pyauto_utils.RemovePath(downloaded_pkg)
  pyauto_utils.RemovePath(downloaded_pkg + '.crdownload')


def GoogleAccountsLogin(test, username, password, tab_index=0, windex=0):
  """Log into Google Accounts.

  Attempts to login to Google by entering the username/password into the google
  login page and click submit button.

  Args:
    test: derived from pyauto.PyUITest - base class for UI test cases.
    username: users login input.
    password: users login password input.
    tab_index: The tab index, default is 0.
    windex: The window index, default is 0.
  """
  test.NavigateToURL('https://www.google.com/accounts/', windex, tab_index)
  email_id = 'document.getElementById("Email").value = "%s"; ' \
             'window.domAutomationController.send("done")' % username
  password = 'document.getElementById("Passwd").value = "%s"; ' \
             'window.domAutomationController.send("done")' % password
  test.ExecuteJavascript(email_id, windex, tab_index);
  test.ExecuteJavascript(password, windex, tab_index);
  test.ExecuteJavascript('document.getElementById("gaia_loginform").submit();'
                         'window.domAutomationController.send("done")',
                         windex, tab_index)


def VerifyGoogleAccountCredsFilled(test, username, password, tab_index=0,
                                   windex=0):
  """Verify stored/saved user and password values to the values in the field.

  Args:
    test: derived from pyauto.PyUITest - base class for UI test cases.
    username: user log in input.
    password: user log in password input.
    tab_index: The tab index, default is 0.
    windex: The window index, default is 0.
  """
  email_value = test.GetDOMValue('document.getElementById("Email").value',
                                 windex, tab_index)
  passwd_value = test.GetDOMValue('document.getElementById("Passwd").value',
                                  windex, tab_index)
  test.assertEqual(email_value, username)
  # Not using assertEqual because if it fails it would end up dumping the
  # password (which is supposed to be private)
  test.assertTrue(passwd_value == password)


def ClearPasswords(test):
  """Clear saved passwords."""
  test.ClearBrowsingData(['PASSWORDS'], 'EVERYTHING')


def Shell2(cmd_string, bg=False):
  """Run a shell command.

  Args:
    cmd_string: command to run
    bg: should the process be run in background? Default: False

  Returns:
    Output, return code
    """
  if not cmd_string: return ('', 0)
  if bg:
    cmd_string += ' 1>/dev/null 2>&1 &'
  proc = os.popen(cmd_string)
  if bg: return ('Background process: %s' % cmd_string, 0)
  out = proc.read()
  retcode = proc.close()
  if not retcode:  # Success
    retcode = 0
  return (out, retcode)


def SendMail(send_from, send_to, subject, text, smtp, file_to_send=None):
  """Send mail to all the group to notify about the crash and uploaded data.

  Args:
    send_from: from mail id.
    send_to: to mail id.
    subject: mail subject.
    text: mail body.
    smtp: The smtp to use.
    file_to_send: attachments for the mail.
  """
  msg = email.MIMEMultipart.MIMEMultipart()
  msg['From'] = send_from
  msg['To'] = send_to
  msg['Date'] = email.Utils.formatdate(localtime=True)
  msg['Subject'] = subject

  # To send multiple files in one message, introduce for loop here for files.
  msg.attach(email.MIMEText.MIMEText(text))
  part = email.MIMEBase.MIMEBase('application', 'octet-stream')
  if file_to_send is not None:
    part.set_payload(open(file_to_send,'rb').read())
    email.Encoders.encode_base64(part)
    part.add_header('Content-Disposition',
                    'attachment; filename="%s"'
                    % os.path.basename(file_to_send))
    msg.attach(part)
  smtp_obj = smtplib.SMTP(smtp)
  smtp_obj.sendmail(send_from, send_to, msg.as_string())
  smtp_obj.close()


def StripUnmatchedKeys(item_to_strip, reference_item):
  """Returns a copy of 'item_to_strip' where unmatched key-value pairs in
  every dictionary are removed.

  This will examine each dictionary in 'item_to_strip' recursively, and will
  remove keys that are not found in the corresponding dictionary in
  'reference_item'. This is useful for testing equality of a subset of data.

  Items may contain dictionaries, lists, or primitives, but only corresponding
  dictionaries will be stripped. A corresponding entry is one which is found
  in the same index in the corresponding parent array or at the same key in the
  corresponding parent dictionary.

  Arg:
    item_to_strip: item to copy and remove all unmatched key-value pairs
    reference_item: item that serves as a reference for which keys-value pairs
                    to strip from 'item_to_strip'

  Returns:
    a copy of 'item_to_strip' where all key-value pairs that do not have a
    matching key in 'reference_item' are removed

  Example:
    item_to_strip = {'tabs': 3,
                     'time': 5908}
    reference_item = {'tabs': 2}
    StripUnmatchedKeys(item_to_strip, reference_item) will return {'tabs': 3}
  """
  def StripList(list1, list2):
    return_list = copy.deepcopy(list2)
    for i in range(min(len(list1), len(list2))):
      return_list[i] = StripUnmatchedKeys(list1[i], list2[i])
    return return_list

  def StripDict(dict1, dict2):
    return_dict = {}
    for key in dict1:
      if key in dict2:
        return_dict[key] = StripUnmatchedKeys(dict1[key], dict2[key])
    return return_dict

  item_to_strip_type = type(item_to_strip)
  if item_to_strip_type is type(reference_item):
    if item_to_strip_type is types.ListType:
      return StripList(item_to_strip, reference_item)
    elif item_to_strip_type is types.DictType:
      return StripDict(item_to_strip, reference_item)
  return copy.deepcopy(item_to_strip)


def StringContentCheck(test, content_string, have_list, nothave_list):
  """Check for the presence or absence of strings within content.
  Confirm all strings in have_list are found in content_strings.
  Confirm all strings in nothave_list are not found in content_strings.

  Args:
    content_string: string to search for within content
    have_list: list of strings found within content
    nothave_list: list of strings not found within content
  """
  for s in have_list:
    test.assertTrue(s in content_string, s)
  for s in nothave_list:
    test.assertTrue(s not in content_string)
