// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/chromeos/login/mock_url_fetchers.h"

#include <errno.h>

#include "base/message_loop.h"
#include "chrome/browser/browser_thread.h"
#include "chrome/common/net/http_return.h"
#include "chrome/common/net/url_fetcher.h"
#include "googleurl/src/gurl.h"
#include "net/url_request/url_request_status.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace chromeos {

ExpectCanceledFetcher::ExpectCanceledFetcher(
    bool success,
    const GURL& url,
    const std::string& results,
    URLFetcher::RequestType request_type,
    URLFetcher::Delegate* d)
    : URLFetcher(url, request_type, d) {
}

ExpectCanceledFetcher::~ExpectCanceledFetcher() {
  task_->Cancel();
}

void ExpectCanceledFetcher::Start() {
  task_ = NewRunnableFunction(&ExpectCanceledFetcher::CompleteFetch);
  BrowserThread::PostDelayedTask(BrowserThread::UI,
                                 FROM_HERE,
                                 task_,
                                 100);
}

// static
void ExpectCanceledFetcher::CompleteFetch() {
  ADD_FAILURE() << "Fetch completed in ExpectCanceledFetcher!";
  MessageLoop::current()->Quit();  // Allow exiting even if we mess up.
}

GotCanceledFetcher::GotCanceledFetcher(bool success,
                                       const GURL& url,
                                       const std::string& results,
                                       URLFetcher::RequestType request_type,
                                       URLFetcher::Delegate* d)
    : URLFetcher(url, request_type, d),
      url_(url) {
}

GotCanceledFetcher::~GotCanceledFetcher() {}

void GotCanceledFetcher::Start() {
  URLRequestStatus status;
  status.set_status(URLRequestStatus::CANCELED);
  delegate()->OnURLFetchComplete(this,
                                 url_,
                                 status,
                                 RC_FORBIDDEN,
                                 ResponseCookies(),
                                 std::string());
}

SuccessFetcher::SuccessFetcher(bool success,
                               const GURL& url,
                               const std::string& results,
                               URLFetcher::RequestType request_type,
                               URLFetcher::Delegate* d)
    : URLFetcher(url, request_type, d),
      url_(url) {
}

SuccessFetcher::~SuccessFetcher() {}

void SuccessFetcher::Start() {
  URLRequestStatus success(URLRequestStatus::SUCCESS, 0);
  delegate()->OnURLFetchComplete(this,
                                 url_,
                                 success,
                                 RC_REQUEST_OK,
                                 ResponseCookies(),
                                 std::string());
}

FailFetcher::FailFetcher(bool success,
                         const GURL& url,
                         const std::string& results,
                         URLFetcher::RequestType request_type,
                         URLFetcher::Delegate* d)
    : URLFetcher(url, request_type, d),
      url_(url) {
}

FailFetcher::~FailFetcher() {}

void FailFetcher::Start() {
  URLRequestStatus failed(URLRequestStatus::FAILED, ECONNRESET);
  delegate()->OnURLFetchComplete(this,
                                 url_,
                                 failed,
                                 RC_REQUEST_OK,
                                 ResponseCookies(),
                                 std::string());
}

HostedFetcher::HostedFetcher(bool success,
                               const GURL& url,
                               const std::string& results,
                               URLFetcher::RequestType request_type,
                               URLFetcher::Delegate* d)
    : URLFetcher(url, request_type, d),
      url_(url) {
}

HostedFetcher::~HostedFetcher() {}

void HostedFetcher::Start() {
  URLRequestStatus success(URLRequestStatus::SUCCESS, 0);
  int response_code = RC_REQUEST_OK;
  std::string data;
  VLOG(1) << upload_data();
  if (upload_data().find("HOSTED") == std::string::npos) {
    VLOG(1) << "HostedFetcher failing request";
    response_code = RC_FORBIDDEN;
    data.assign("Error=BadAuthentication");
  }
  delegate()->OnURLFetchComplete(this,
                                 url_,
                                 success,
                                 response_code,
                                 ResponseCookies(),
                                 data);
}

}  // namespace chromeos
