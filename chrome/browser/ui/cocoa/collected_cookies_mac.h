// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import <Cocoa/Cocoa.h>

#include "base/cocoa_protocols_mac.h"
#include "base/scoped_nsobject.h"
#include "base/scoped_ptr.h"
#include "chrome/browser/cookies_tree_model.h"
#include "chrome/browser/ui/cocoa/constrained_window_mac.h"
#import "chrome/browser/ui/cocoa/cookie_tree_node.h"
#include "chrome/common/notification_registrar.h"

@class CollectedCookiesWindowController;
@class VerticalGradientView;
class TabContents;

// The constrained window delegate reponsible for managing the collected
// cookies dialog.
class CollectedCookiesMac : public ConstrainedWindowMacDelegateCustomSheet,
                            public NotificationObserver {
 public:
  CollectedCookiesMac(NSWindow* parent, TabContents* tab_contents);

  void OnSheetDidEnd(NSWindow* sheet);

  // ConstrainedWindowMacDelegateCustomSheet implementation.
  virtual void DeleteDelegate();

 private:
  virtual ~CollectedCookiesMac();

  // NotificationObserver implementation.
  void Observe(NotificationType type,
               const NotificationSource& source,
               const NotificationDetails& details);

  NotificationRegistrar registrar_;

  ConstrainedWindow* window_;

  TabContents* tab_contents_;

  CollectedCookiesWindowController* sheet_controller_;

  DISALLOW_COPY_AND_ASSIGN(CollectedCookiesMac);
};

// Controller for the collected cookies dialog. This class stores an internal
// copy of the CookiesTreeModel but with Cocoa-converted values (NSStrings and
// NSImages instead of std::strings and SkBitmaps). Doing this allows us to use
// bindings for the interface. Changes are pushed to this internal model via a
// very thin bridge (see cookies_window_controller.h).
@interface CollectedCookiesWindowController : NSWindowController
                                              <NSOutlineViewDelegate,
                                               NSWindowDelegate> {
 @private
  // Platform-independent model.
  scoped_ptr<CookiesTreeModel> allowedTreeModel_;
  scoped_ptr<CookiesTreeModel> blockedTreeModel_;

  // Cached array of icons.
  scoped_nsobject<NSMutableArray> icons_;

  // Our Cocoa copy of the model.
  scoped_nsobject<CocoaCookieTreeNode> cocoaAllowedTreeModel_;
  scoped_nsobject<CocoaCookieTreeNode> cocoaBlockedTreeModel_;

  BOOL allowedCookiesButtonsEnabled_;
  BOOL blockedCookiesButtonsEnabled_;

  IBOutlet NSTreeController* allowedTreeController_;
  IBOutlet NSTreeController* blockedTreeController_;
  IBOutlet NSOutlineView* allowedOutlineView_;
  IBOutlet NSOutlineView* blockedOutlineView_;
  IBOutlet VerticalGradientView* infoBar_;
  IBOutlet NSImageView* infoBarIcon_;
  IBOutlet NSTextField* infoBarText_;
  IBOutlet NSSplitView* splitView_;
  IBOutlet NSScrollView* lowerScrollView_;
  IBOutlet NSTextField* blockedCookiesText_;

  scoped_nsobject<NSViewAnimation> animation_;

  TabContents* tabContents_;  // weak

  BOOL infoBarVisible_;
}
@property (readonly, nonatomic) NSTreeController* allowedTreeController;
@property (readonly, nonatomic) NSTreeController* blockedTreeController;

@property (assign, nonatomic) BOOL allowedCookiesButtonsEnabled;
@property (assign, nonatomic) BOOL blockedCookiesButtonsEnabled;

// Designated initializer. TabContents cannot be NULL.
- (id)initWithTabContents:(TabContents*)tabContents;

// Closes the sheet and ends the modal loop. This will also cleanup the memory.
- (IBAction)closeSheet:(id)sender;

- (IBAction)allowOrigin:(id)sender;
- (IBAction)allowForSessionFromOrigin:(id)sender;
- (IBAction)blockOrigin:(id)sender;

// NSSplitView delegate methods:
- (CGFloat)    splitView:(NSSplitView *)sender
  constrainMinCoordinate:(CGFloat)proposedMin
             ofSubviewAt:(NSInteger)offset;
- (BOOL)splitView:(NSSplitView *)sender canCollapseSubview:(NSView *)subview;

// Returns the cocoaAllowedTreeModel_ and cocoaBlockedTreeModel_.
- (CocoaCookieTreeNode*)cocoaAllowedTreeModel;
- (CocoaCookieTreeNode*)cocoaBlockedTreeModel;
- (void)setCocoaAllowedTreeModel:(CocoaCookieTreeNode*)model;
- (void)setCocoaBlockedTreeModel:(CocoaCookieTreeNode*)model;

// Returns the allowedTreeModel_ and blockedTreeModel_.
- (CookiesTreeModel*)allowedTreeModel;
- (CookiesTreeModel*)blockedTreeModel;

- (void)loadTreeModelFromTabContents;
@end
