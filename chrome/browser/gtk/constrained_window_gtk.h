// Copyright (c) 2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_GTK_CONSTRAINED_WINDOW_GTK_H_
#define CHROME_BROWSER_GTK_CONSTRAINED_WINDOW_GTK_H_
#pragma once

#include <gtk/gtk.h>

#include "app/gtk_signal.h"
#include "base/basictypes.h"
#include "base/task.h"
#include "chrome/browser/gtk/owned_widget_gtk.h"
#include "chrome/browser/tab_contents/constrained_window.h"

class TabContents;
typedef struct _GdkColor GdkColor;
#if defined(TOUCH_UI)
class TabContentsViewViews;
#else
class TabContentsViewGtk;
#endif

class ConstrainedWindowGtkDelegate {
 public:
  // Returns the widget that will be put in the constrained window's container.
  virtual GtkWidget* GetWidgetRoot() = 0;

  // Tells the delegate to either delete itself or set up a task to delete
  // itself later.
  virtual void DeleteDelegate() = 0;

  virtual bool GetBackgroundColor(GdkColor* color);

 protected:
  virtual ~ConstrainedWindowGtkDelegate();
};

// Constrained window implementation for the GTK port. Unlike the Win32 system,
// ConstrainedWindowGtk doesn't draw draggable fake windows and instead just
// centers the dialog. It is thus an order of magnitude simpler.
class ConstrainedWindowGtk : public ConstrainedWindow {
 public:
#if defined(TOUCH_UI)
   typedef TabContentsViewViews TabContentsViewType;
#else
   typedef TabContentsViewGtk TabContentsViewType;
#endif

  virtual ~ConstrainedWindowGtk();

  // Overridden from ConstrainedWindow:
  virtual void ShowConstrainedWindow();
  virtual void CloseConstrainedWindow();

  // Returns the TabContents that constrains this Constrained Window.
  TabContents* owner() const { return owner_; }

  // Returns the toplevel widget that displays this "window".
  GtkWidget* widget() { return border_.get(); }

  // Returns the View that we collaborate with to position ourselves.
  TabContentsViewType* ContainingView();

 private:
  friend class ConstrainedWindow;

  ConstrainedWindowGtk(TabContents* owner,
                       ConstrainedWindowGtkDelegate* delegate);

  // Handler for Escape.
  CHROMEGTK_CALLBACK_1(ConstrainedWindowGtk, gboolean, OnKeyPress,
                       GdkEventKey*);

  // The TabContents that owns and constrains this ConstrainedWindow.
  TabContents* owner_;

  // The top level widget container that exports to our TabContentsView.
  OwnedWidgetGtk border_;

  // Delegate that provides the contents of this constrained window.
  ConstrainedWindowGtkDelegate* delegate_;

  // Stores if |ShowConstrainedWindow()| has been called.
  bool visible_;

  ScopedRunnableMethodFactory<ConstrainedWindowGtk> factory_;

  DISALLOW_COPY_AND_ASSIGN(ConstrainedWindowGtk);
};

#endif  // CHROME_BROWSER_GTK_CONSTRAINED_WINDOW_GTK_H_
