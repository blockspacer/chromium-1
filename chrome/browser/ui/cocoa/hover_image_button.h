// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import <Cocoa/Cocoa.h>

#include "base/scoped_nsobject.h"
#include "chrome/browser/ui/cocoa/hover_button.h"

// A button that changes images when you hover over it and click it.
@interface HoverImageButton : HoverButton {
 @private
  float defaultOpacity_;
  float hoverOpacity_;
  float pressedOpacity_;

  scoped_nsobject<NSImage> defaultImage_;
  scoped_nsobject<NSImage> hoverImage_;
  scoped_nsobject<NSImage> pressedImage_;
}

// Sets the default image.
- (void)setDefaultImage:(NSImage*)image;

// Sets the hover image.
- (void)setHoverImage:(NSImage*)image;

// Sets the pressed image.
- (void)setPressedImage:(NSImage*)image;

// Sets the default opacity.
- (void)setDefaultOpacity:(float)opacity;

// Sets the opacity on hover.
- (void)setHoverOpacity:(float)opacity;

// Sets the opacity when pressed.
- (void)setPressedOpacity:(float)opacity;

@end
