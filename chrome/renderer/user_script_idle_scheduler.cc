// Copyright (c) 2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/renderer/user_script_idle_scheduler.h"

#include "base/message_loop.h"
#include "chrome/renderer/render_view.h"

namespace {
// The length of time to wait after the DOM is complete to try and run user
// scripts.
const int kUserScriptIdleTimeoutMs = 200;
}

UserScriptIdleScheduler::UserScriptIdleScheduler(RenderView* view,
                                                 WebKit::WebFrame* frame)
    : ALLOW_THIS_IN_INITIALIZER_LIST(method_factory_(this)), view_(view),
  frame_(frame), has_run_(false) {
}

UserScriptIdleScheduler::~UserScriptIdleScheduler() {
}

void UserScriptIdleScheduler::DidFinishDocumentLoad() {
  MessageLoop::current()->PostDelayedTask(FROM_HERE,
      method_factory_.NewRunnableMethod(&UserScriptIdleScheduler::MaybeRun),
      kUserScriptIdleTimeoutMs);
}

void UserScriptIdleScheduler::DidFinishLoad() {
  // Ensure that running scripts does not keep any progress UI running.
  MessageLoop::current()->PostTask(FROM_HERE,
      method_factory_.NewRunnableMethod(&UserScriptIdleScheduler::MaybeRun));
}

void UserScriptIdleScheduler::Cancel() {
  view_ = NULL;
  frame_ = NULL;
}

void UserScriptIdleScheduler::MaybeRun() {
  if (!view_ || has_run())
    return;

  // Note: we must set this before calling OnUserScriptIdleTriggered, because
  // that may result in a synchronous call back into MaybeRun if there is a
  // pending task currently in the queue.
  // http://code.google.com/p/chromium/issues/detail?id=29644
  has_run_ = true;

  DCHECK(frame_);
  view_->OnUserScriptIdleTriggered(frame_);
  Cancel();
}
