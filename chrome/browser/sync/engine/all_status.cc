// Copyright (c) 2006-2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/sync/engine/all_status.h"

#include <algorithm>

#include "base/logging.h"
#include "base/port.h"
#include "chrome/browser/sync/engine/net/server_connection_manager.h"
#include "chrome/browser/sync/protocol/service_constants.h"
#include "chrome/browser/sync/sessions/session_state.h"
#include "chrome/browser/sync/syncable/directory_manager.h"

namespace browser_sync {

static const sync_api::SyncManager::Status init_status =
  { sync_api::SyncManager::Status::OFFLINE };

AllStatus::AllStatus() : status_(init_status) {
  status_.initial_sync_ended = true;
  status_.notifications_enabled = false;
}

AllStatus::~AllStatus() {
}

sync_api::SyncManager::Status AllStatus::CreateBlankStatus() const {
  // Status is initialized with the previous status value.  Variables
  // whose values accumulate (e.g. lifetime counters like updates_received)
  // are not to be cleared here.
  sync_api::SyncManager::Status status = status_;
  status.syncing = true;
  status.unsynced_count = 0;
  status.conflicting_count = 0;
  status.initial_sync_ended = false;
  status.syncer_stuck = false;
  status.max_consecutive_errors = 0;
  status.server_broken = false;
  status.updates_available = 0;
  return status;
}

sync_api::SyncManager::Status AllStatus::CalcSyncing(
    const SyncEngineEvent &event) const {
  sync_api::SyncManager::Status status = CreateBlankStatus();
  const sessions::SyncSessionSnapshot* snapshot = event.snapshot;
  status.unsynced_count += static_cast<int>(snapshot->unsynced_count);
  status.conflicting_count += snapshot->errors.num_conflicting_commits;
  // The syncer may not be done yet, which could cause conflicting updates.
  // But this is only used for status, so it is better to have visibility.
  status.conflicting_count += snapshot->num_conflicting_updates;

  status.syncing |= snapshot->syncer_status.syncing;
  status.syncing = snapshot->has_more_to_sync && snapshot->is_silenced;
  status.initial_sync_ended |= snapshot->is_share_usable;
  status.syncer_stuck |= snapshot->syncer_status.syncer_stuck;

  const sessions::ErrorCounters& errors(snapshot->errors);
  if (errors.consecutive_errors > status.max_consecutive_errors)
    status.max_consecutive_errors = errors.consecutive_errors;

  // 100 is an arbitrary limit.
  if (errors.consecutive_transient_error_commits > 100)
    status.server_broken = true;

  status.updates_available += snapshot->num_server_changes_remaining;

  // Accumulate update count only once per session to avoid double-counting.
  // TODO(ncarter): Make this realtime by having the syncer_status
  // counter preserve its value across sessions.  http://crbug.com/26339
  if (event.what_happened == SyncEngineEvent::SYNC_CYCLE_ENDED) {
    status.updates_received +=
        snapshot->syncer_status.num_updates_downloaded_total;
    status.tombstone_updates_received +=
        snapshot->syncer_status.num_tombstone_updates_downloaded_total;
  }
  return status;
}

void AllStatus::CalcStatusChanges() {
  const bool unsynced_changes = status_.unsynced_count > 0;
  const bool online = status_.authenticated &&
    status_.server_reachable && status_.server_up && !status_.server_broken;
  if (online) {
    if (status_.syncer_stuck)
      status_.summary = sync_api::SyncManager::Status::CONFLICT;
    else if (unsynced_changes || status_.syncing)
      status_.summary = sync_api::SyncManager::Status::SYNCING;
    else
      status_.summary = sync_api::SyncManager::Status::READY;
  } else if (!status_.initial_sync_ended) {
    status_.summary = sync_api::SyncManager::Status::OFFLINE_UNUSABLE;
  } else if (unsynced_changes) {
    status_.summary = sync_api::SyncManager::Status::OFFLINE_UNSYNCED;
  } else {
    status_.summary = sync_api::SyncManager::Status::OFFLINE;
  }
}

void AllStatus::OnSyncEngineEvent(const SyncEngineEvent& event) {
  ScopedStatusLock lock(this);
  switch (event.what_happened) {
    case SyncEngineEvent::SYNC_CYCLE_ENDED:
    case SyncEngineEvent::STATUS_CHANGED:
      status_ = CalcSyncing(event);
      break;
    case SyncEngineEvent::SYNCER_THREAD_PAUSED:
    case SyncEngineEvent::SYNCER_THREAD_RESUMED:
    case SyncEngineEvent::SYNCER_THREAD_WAITING_FOR_CONNECTION:
    case SyncEngineEvent::SYNCER_THREAD_CONNECTED:
    case SyncEngineEvent::STOP_SYNCING_PERMANENTLY:
    case SyncEngineEvent::SYNCER_THREAD_EXITING:
       break;
    default:
      LOG(ERROR) << "Unrecognized Syncer Event: " << event.what_happened;
      break;
  }
}

void AllStatus::HandleServerConnectionEvent(
    const ServerConnectionEvent& event) {
  if (ServerConnectionEvent::STATUS_CHANGED == event.what_happened) {
    ScopedStatusLock lock(this);
    status_.server_up = IsGoodReplyFromServer(event.connection_code);
    status_.server_reachable = event.server_reachable;

    if (event.connection_code == HttpResponse::SERVER_CONNECTION_OK) {
      if (!status_.authenticated) {
        status_ = CreateBlankStatus();
      }
      status_.authenticated = true;
    } else {
      status_.authenticated = false;
    }
  }
}

sync_api::SyncManager::Status AllStatus::status() const {
  AutoLock lock(mutex_);
  return status_;
}

void AllStatus::SetNotificationsEnabled(bool notifications_enabled) {
  ScopedStatusLock lock(this);
  status_.notifications_enabled = notifications_enabled;
}

void AllStatus::IncrementNotificationsSent() {
  ScopedStatusLock lock(this);
  ++status_.notifications_sent;
}

void AllStatus::IncrementNotificationsReceived() {
  ScopedStatusLock lock(this);
  ++status_.notifications_received;
}

ScopedStatusLock::ScopedStatusLock(AllStatus* allstatus)
    : allstatus_(allstatus) {
  allstatus->mutex_.Acquire();
}

ScopedStatusLock::~ScopedStatusLock() {
  allstatus_->CalcStatusChanges();
  allstatus_->mutex_.Release();
}

}  // namespace browser_sync
