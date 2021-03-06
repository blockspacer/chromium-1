// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_BROWSING_DATA_REMOVER_H_
#define CHROME_BROWSER_BROWSING_DATA_REMOVER_H_
#pragma once

#include <vector>

#include "base/observer_list.h"
#include "base/ref_counted.h"
#include "base/time.h"
#include "chrome/browser/appcache/chrome_appcache_service.h"
#include "chrome/browser/cancelable_request.h"
#include "chrome/common/notification_registrar.h"

class PluginDataRemover;
class Profile;
class URLRequestContextGetter;

namespace disk_cache {
class Backend;
}

namespace webkit_database {
class DatabaseTracker;
}

// BrowsingDataRemover is responsible for removing data related to browsing:
// visits in url database, downloads, cookies ...

class BrowsingDataRemover : public NotificationObserver {
 public:
  // Time period ranges available when doing browsing data removals.
  enum TimePeriod {
    LAST_HOUR = 0,
    LAST_DAY,
    LAST_WEEK,
    FOUR_WEEKS,
    EVERYTHING
  };

  // Mask used for Remove.

  // In addition to visits, this removes keywords and the last session.
  static const int REMOVE_HISTORY = 1 << 0;
  static const int REMOVE_DOWNLOADS = 1 << 1;
  static const int REMOVE_COOKIES = 1 << 2;
  static const int REMOVE_PASSWORDS = 1 << 3;
  static const int REMOVE_FORM_DATA = 1 << 4;
  static const int REMOVE_CACHE = 1 << 5;
  static const int REMOVE_LSO_DATA = 1 << 6;

  // Observer is notified when the removal is done. Done means keywords have
  // been deleted, cache cleared and all other tasks scheduled.
  class Observer {
   public:
    virtual void OnBrowsingDataRemoverDone() = 0;

   protected:
    virtual ~Observer() {}
  };

  // Creates a BrowsingDataRemover to remove browser data from the specified
  // profile in the specified time range. Use Remove to initiate the removal.
  BrowsingDataRemover(Profile* profile, base::Time delete_begin,
                      base::Time delete_end);

  // Creates a BrowsingDataRemover to remove browser data from the specified
  // profile in the specified time range.
  BrowsingDataRemover(Profile* profile, TimePeriod time_period,
                      base::Time delete_end);

  // Removes the specified items related to browsing.
  void Remove(int remove_mask);

  void AddObserver(Observer* observer);
  void RemoveObserver(Observer* observer);

  // Called when history deletion is done.
  void OnHistoryDeletionDone();

  static bool is_removing() { return removing_; }

 private:
  enum CacheState {
    STATE_NONE,
    STATE_CREATE_MAIN,
    STATE_CREATE_MEDIA,
    STATE_DELETE_MAIN,
    STATE_DELETE_MEDIA,
    STATE_DONE
  };

  // BrowsingDataRemover deletes itself (using DeleteTask) and is not supposed
  // to be deleted by other objects so make destructor private and DeleteTask
  // a friend.
  friend class DeleteTask<BrowsingDataRemover>;
  ~BrowsingDataRemover();

  // NotificationObserver method. Callback when TemplateURLModel has finished
  // loading. Deletes the entries from the model, and if we're not waiting on
  // anything else notifies observers and deletes this BrowsingDataRemover.
  virtual void Observe(NotificationType type,
                       const NotificationSource& source,
                       const NotificationDetails& details);

  // If we're not waiting on anything, notifies observers and deletes this
  // object.
  void NotifyAndDeleteIfDone();

  // Callback when the cache has been deleted. Invokes NotifyAndDeleteIfDone.
  void ClearedCache();

  // Invoked on the IO thread to delete from the cache.
  void ClearCacheOnIOThread();

  // Performs the actual work to delete the cache.
  void DoClearCache(int rv);

  // Callback when HTML5 databases have been deleted. Invokes
  // NotifyAndDeleteIfDone.
  void OnClearedDatabases(int rv);

  // Invoked on the FILE thread to delete HTML5 databases. Ignores any within
  // the |webkit_db_whitelist|.
  void ClearDatabasesOnFILEThread(base::Time delete_begin,
      const std::vector<string16>& webkit_db_whitelist);

  // Callback when the appcache has been cleared. Invokes
  // NotifyAndDeleteIfDone.
  void OnClearedAppCache();

  // Invoked on the IO thread to delete from the AppCache, ignoring data from
  // any origins within the |origin_whitelist|.
  void ClearAppCacheOnIOThread(base::Time delete_begin,
                               const std::vector<GURL>& origin_whitelist);

  // Lower level helpers.
  void OnGotAppCacheInfo(int rv);
  void OnAppCacheDeleted(int rv);
  ChromeAppCacheService* GetAppCacheService();

  // Callback when plug-in data has been cleared. Invokes NotifyAndDeleteIfDone.
  void OnClearedPluginData();

  // Calculate the begin time for the deletion range specified by |time_period|.
  base::Time CalculateBeginDeleteTime(TimePeriod time_period);

  // Returns true if we're all done.
  bool all_done() {
    return registrar_.IsEmpty() && !waiting_for_clear_cache_ &&
           !waiting_for_clear_history_ && !waiting_for_clear_databases_ &&
           !waiting_for_clear_appcache_ && !waiting_for_clear_lso_data_;
  }

  NotificationRegistrar registrar_;

  // Profile we're to remove from.
  Profile* profile_;

  // Start time to delete from.
  const base::Time delete_begin_;

  // End time to delete to.
  const base::Time delete_end_;

  // True if Remove has been invoked.
  static bool removing_;

  // Reference to database tracker held while deleting databases.
  scoped_refptr<webkit_database::DatabaseTracker> database_tracker_;

  net::CompletionCallbackImpl<BrowsingDataRemover> database_cleared_callback_;
  net::CompletionCallbackImpl<BrowsingDataRemover> cache_callback_;

  // Used to clear the appcache.
  net::CompletionCallbackImpl<BrowsingDataRemover> appcache_got_info_callback_;
  net::CompletionCallbackImpl<BrowsingDataRemover> appcache_deleted_callback_;
  scoped_refptr<appcache::AppCacheInfoCollection> appcache_info_;
  scoped_refptr<URLRequestContextGetter> request_context_getter_;
  std::vector<GURL> appcache_whitelist_;
  int appcaches_to_be_deleted_count_;

  // Used to delete data from the HTTP caches.
  CacheState next_cache_state_;
  disk_cache::Backend* cache_;
  scoped_refptr<URLRequestContextGetter> main_context_getter_;
  scoped_refptr<URLRequestContextGetter> media_context_getter_;

  // Used to delete plugin data.
  scoped_refptr<PluginDataRemover> plugin_data_remover_;

  // True if we're waiting for various data to be deleted.
  bool waiting_for_clear_databases_;
  bool waiting_for_clear_history_;
  bool waiting_for_clear_cache_;
  bool waiting_for_clear_appcache_;
  bool waiting_for_clear_lso_data_;

  ObserverList<Observer> observer_list_;

  // Used if we need to clear history.
  CancelableRequestConsumer request_consumer_;

  DISALLOW_COPY_AND_ASSIGN(BrowsingDataRemover);
};

#endif  // CHROME_BROWSER_BROWSING_DATA_REMOVER_H_
