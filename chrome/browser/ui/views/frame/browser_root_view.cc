// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/views/frame/browser_root_view.h"

#include "app/drag_drop_types.h"
#include "app/l10n_util.h"
#include "app/os_exchange_data.h"
#include "chrome/browser/autocomplete/autocomplete.h"
#include "chrome/browser/autocomplete/autocomplete_classifier.h"
#include "chrome/browser/autocomplete/autocomplete_match.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/omnibox/location_bar.h"
#include "chrome/browser/ui/views/frame/browser_view.h"
#include "chrome/browser/ui/views/frame/browser_frame.h"
#include "chrome/browser/ui/views/tabs/tab_strip.h"
#include "grit/chromium_strings.h"

BrowserRootView::BrowserRootView(BrowserView* browser_view,
                                 views::Widget* widget)
    : views::RootView(widget),
      browser_view_(browser_view),
      forwarding_to_tab_strip_(false) {
  SetAccessibleName(l10n_util::GetString(IDS_PRODUCT_NAME));
}

bool BrowserRootView::GetDropFormats(
      int* formats,
      std::set<OSExchangeData::CustomFormat>* custom_formats) {
  if (tabstrip() && tabstrip()->IsVisible() && !tabstrip()->IsAnimating()) {
    *formats = OSExchangeData::URL | OSExchangeData::STRING;
    return true;
  }
  return false;
}

bool BrowserRootView::AreDropTypesRequired() {
  return true;
}

bool BrowserRootView::CanDrop(const OSExchangeData& data) {
  if (!tabstrip() || !tabstrip()->IsVisible() || tabstrip()->IsAnimating())
    return false;

  // If there is a URL, we'll allow the drop.
  if (data.HasURL())
    return true;

  // If there isn't a URL, see if we can 'paste and go'.
  return GetPasteAndGoURL(data, NULL);
}

void BrowserRootView::OnDragEntered(const views::DropTargetEvent& event) {
  if (ShouldForwardToTabStrip(event)) {
    forwarding_to_tab_strip_ = true;
    scoped_ptr<views::DropTargetEvent> mapped_event(
        MapEventToTabStrip(event, event.GetData()));
    tabstrip()->OnDragEntered(*mapped_event.get());
  }
}

int BrowserRootView::OnDragUpdated(const views::DropTargetEvent& event) {
  if (ShouldForwardToTabStrip(event)) {
    scoped_ptr<views::DropTargetEvent> mapped_event(
        MapEventToTabStrip(event, event.GetData()));
    if (!forwarding_to_tab_strip_) {
      tabstrip()->OnDragEntered(*mapped_event.get());
      forwarding_to_tab_strip_ = true;
    }
    return tabstrip()->OnDragUpdated(*mapped_event.get());
  } else if (forwarding_to_tab_strip_) {
    forwarding_to_tab_strip_ = false;
    tabstrip()->OnDragExited();
  }
  return DragDropTypes::DRAG_NONE;
}

void BrowserRootView::OnDragExited() {
  if (forwarding_to_tab_strip_) {
    forwarding_to_tab_strip_ = false;
    tabstrip()->OnDragExited();
  }
}

int BrowserRootView::OnPerformDrop(const views::DropTargetEvent& event) {
  if (!forwarding_to_tab_strip_)
    return DragDropTypes::DRAG_NONE;

  // Extract the URL and create a new OSExchangeData containing the URL. We do
  // this as the TabStrip doesn't know about the autocomplete edit and neeeds
  // to know about it to handle 'paste and go'.
  GURL url;
  std::wstring title;
  OSExchangeData mapped_data;
  if (!event.GetData().GetURLAndTitle(&url, &title) || !url.is_valid()) {
    // The url isn't valid. Use the paste and go url.
    if (GetPasteAndGoURL(event.GetData(), &url))
      mapped_data.SetURL(url, std::wstring());
    // else case: couldn't extract a url or 'paste and go' url. This ends up
    // passing through an OSExchangeData with nothing in it. We need to do this
    // so that the tab strip cleans up properly.
  } else {
    mapped_data.SetURL(url, std::wstring());
  }
  forwarding_to_tab_strip_ = false;
  scoped_ptr<views::DropTargetEvent> mapped_event(
      MapEventToTabStrip(event, mapped_data));
  return tabstrip()->OnPerformDrop(*mapped_event);
}

bool BrowserRootView::ShouldForwardToTabStrip(
    const views::DropTargetEvent& event) {
  if (!tabstrip()->IsVisible())
    return false;

  // Allow the drop as long as the mouse is over the tabstrip or vertically
  // before it.
  gfx::Point tab_loc_in_host;
  ConvertPointToView(tabstrip(), this, &tab_loc_in_host);
  return event.y() < tab_loc_in_host.y() + tabstrip()->height();
}

views::DropTargetEvent* BrowserRootView::MapEventToTabStrip(
    const views::DropTargetEvent& event,
    const OSExchangeData& data) {
  gfx::Point tab_strip_loc(event.location());
  ConvertPointToView(this, tabstrip(), &tab_strip_loc);
  return new views::DropTargetEvent(data, tab_strip_loc.x(),
                                    tab_strip_loc.y(),
                                    event.GetSourceOperations());
}

BaseTabStrip* BrowserRootView::tabstrip() const {
  return browser_view_->tabstrip();
}

bool BrowserRootView::GetPasteAndGoURL(const OSExchangeData& data, GURL* url) {
  if (!data.HasString())
    return false;

  std::wstring text;
  if (!data.GetString(&text) || text.empty())
    return false;

  AutocompleteMatch match;
  browser_view_->browser()->profile()->GetAutocompleteClassifier()->Classify(
      text, std::wstring(), false, &match, NULL);
  if (!match.destination_url.is_valid())
    return false;

  if (url)
    *url = match.destination_url;
  return true;
}
