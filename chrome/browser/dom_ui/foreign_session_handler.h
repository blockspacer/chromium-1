// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_DOM_UI_FOREIGN_SESSION_HANDLER_H_
#define CHROME_BROWSER_DOM_UI_FOREIGN_SESSION_HANDLER_H_
#pragma once

#include <vector>

#include "chrome/browser/dom_ui/dom_ui.h"
#include "chrome/browser/sessions/session_service.h"
#include "chrome/browser/sync/glue/session_model_associator.h"
#include "chrome/common/notification_observer.h"
#include "chrome/common/notification_registrar.h"

namespace browser_sync {

class ForeignSessionHandler : public DOMMessageHandler,
                              public NotificationObserver {
 public:
  // DOMMessageHandler implementation.
  virtual void RegisterMessages();

  ForeignSessionHandler();
  virtual ~ForeignSessionHandler() {}

 private:
  // Used to register ForeignSessionHandler for notifications.
  void Init();

  // Determines how ForeignSessionHandler will interact with the new tab page.
  virtual void Observe(NotificationType type,
                       const NotificationSource& source,
                       const NotificationDetails& details);

  // Returns a pointer to the current session model associator or NULL.
  SessionModelAssociator* GetModelAssociator();

  // Determines whether foreign sessions should be obtained from the sync model.
  // This is a javascript callback handler, and it is also called when the sync
  // model has changed and the new tab page needs to reflect the changes.
  void HandleGetForeignSessions(const ListValue* args);

  // Helper for reopening a foreign session in a new browser window.
  void OpenForeignSession(SessionModelAssociator* associator, int64 id);

  // Helper for listing the foreign sessions on the new tab page.
  void GetForeignSessions(SessionModelAssociator* associator);

  // Determines which session is to be opened, and then calls
  // OpenForeignSession, to begin the process of opening a new browser window.
  // This is a javascript callback handler.
  void HandleReopenForeignSession(const ListValue* args);

  // The Registrar used to register ForeignSessionHandler for notifications.
  NotificationRegistrar registrar_;

  DISALLOW_COPY_AND_ASSIGN(ForeignSessionHandler);
};

}  // namespace browser_sync

#endif  // CHROME_BROWSER_DOM_UI_FOREIGN_SESSION_HANDLER_H_
