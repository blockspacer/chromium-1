// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_RENDERER_HOST_RESOURCE_MESSAGE_FILTER_H_
#define CHROME_BROWSER_RENDERER_HOST_RESOURCE_MESSAGE_FILTER_H_

#include "base/scoped_ptr.h"
#include "chrome/browser/browser_message_filter.h"
#include "chrome/common/child_process_info.h"
#include "webkit/glue/resource_type.h"

class ChromeURLRequestContext;
class ResourceDispatcherHost;
class URLRequestContext;

// This class filters out incoming IPC messages for network requests and
// processes them on the IPC thread.  As a result, network requests are not
// delayed by costly UI processing that may be occuring on the main thread of
// the browser.  It also means that any hangs in starting a network request
// will not interfere with browser UI.
class ResourceMessageFilter : public BrowserMessageFilter {
 public:
  // Allows overriding the URLRequestContext used to service requests.
  class URLRequestContextOverride
      : public base::RefCountedThreadSafe<URLRequestContextOverride> {
   public:
    URLRequestContextOverride() {}

    virtual URLRequestContext* GetRequestContext(
        uint32 request_id, ResourceType::Type resource_type) = 0;

   protected:
    friend class base::RefCountedThreadSafe<URLRequestContextOverride>;
    virtual ~URLRequestContextOverride() {}

    DISALLOW_COPY_AND_ASSIGN(URLRequestContextOverride);
  };

  ResourceMessageFilter(int child_id,
                        ChildProcessInfo::ProcessType process_type,
                        ResourceDispatcherHost* resource_dispatcher_host);

  // BrowserMessageFilter implementation.
  virtual void OnChannelClosing();
  virtual bool OnMessageReceived(const IPC::Message& message,
                                 bool* message_was_ok);

  // Returns the URLRequestContext for the given request.
  ChromeURLRequestContext* GetURLRequestContext(
      uint32 request_id, ResourceType::Type resource_type);

  void set_url_request_context_override(URLRequestContextOverride* u) {
    url_request_context_override_ = u;
  }

  int child_id() const { return child_id_; }
  ChildProcessInfo::ProcessType process_type() const { return process_type_; }

 protected:
  // Protected destructor so that we can be overriden in tests.
  virtual ~ResourceMessageFilter();

 private:
  // The ID of the child process.
  int child_id_;

  ChildProcessInfo::ProcessType process_type_;

  // Owned by BrowserProcess, which is guaranteed to outlive us.
  ResourceDispatcherHost* resource_dispatcher_host_;

  scoped_refptr<URLRequestContextOverride> url_request_context_override_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(ResourceMessageFilter);
};

#endif  // CHROME_BROWSER_RENDERER_HOST_RESOURCE_MESSAGE_FILTER_H_
