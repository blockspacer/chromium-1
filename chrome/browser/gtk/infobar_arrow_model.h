// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_GTK_INFOBAR_ARROW_MODEL_H_
#define CHROME_BROWSER_GTK_INFOBAR_ARROW_MODEL_H_

#include <gtk/gtk.h>

#include "app/animation_delegate.h"
#include "app/slide_animation.h"
#include "third_party/skia/include/core/SkPaint.h"

namespace gfx {
class Point;
}

class InfoBar;

// A helper class that tracks the state of an infobar arrow and provides a
// utility to draw it.
class InfoBarArrowModel : public AnimationDelegate {
 public:
  class Observer {
   public:
    // The arrow has changed states; relevant widgets need to be repainted.
    virtual void PaintStateChanged() = 0;
  };

  explicit InfoBarArrowModel(Observer* observer);
  virtual ~InfoBarArrowModel();

  // An infobar has been added or removed that will affect the state of this
  // arrow.
  void ShowArrowFor(InfoBar* bar, bool animate);

  // Returns true if the arrow is showing at all.
  bool NeedToDrawInfoBarArrow();

  // Paints the arrow on |widget|, in response to |expose|, with the bottom
  // center of the arrow at |origin|, drawing a border with |border_color|.
  void Paint(GtkWidget* widget,
             GdkEventExpose* expose,
             const gfx::Point& origin,
             const GdkColor& border_color);

  // Overridden from AnimationDelegate.
  virtual void AnimationEnded(const Animation* animation);
  virtual void AnimationProgressed(const Animation* animation);
  virtual void AnimationCanceled(const Animation* animation);

 private:
  // A pair of colors used to draw a gradient for an arrow.
  struct InfoBarColors {
    SkColor top;
    SkColor bottom;
  };

  // Calculates the currently showing arrow color, which is a blend of the new
  // arrow color and the old arrow color.
  InfoBarColors CurrentInfoBarColors();

  // The view that owns us.
  Observer* observer_;

  // An animation that tracks the progress of the transition from the last color
  // to the new color.
  SlideAnimation animation_;

  // The color we are animating towards.
  InfoBarColors target_colors_;
  // The last color we showed (the one we are animating away from).
  InfoBarColors previous_colors_;

  DISALLOW_COPY_AND_ASSIGN(InfoBarArrowModel);
};

#endif  // CHROME_BROWSER_GTK_INFOBAR_ARROW_MODEL_H_
