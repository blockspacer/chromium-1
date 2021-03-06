// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/gtk/collected_cookies_gtk.h"

#include <string>

#include "app/l10n_util.h"
#include "chrome/browser/cookies_tree_model.h"
#include "chrome/browser/gtk/gtk_util.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/tab_contents/tab_contents.h"
#include "chrome/common/notification_source.h"
#include "grit/generated_resources.h"

namespace {
// Width and height of the cookie tree view.
const int kTreeViewWidth = 450;
const int kTreeViewHeight = 150;

// Padding within the banner box.
const int kBannerPadding = 3;

// Returns the text to display in the info bar when content settings were
// created.
const std::string GetInfobarLabel(ContentSetting setting,
                                  bool multiple_domains_added,
                                  const string16& domain_name) {
  if (multiple_domains_added) {
    switch (setting) {
      case CONTENT_SETTING_BLOCK:
        return l10n_util::GetStringUTF8(
            IDS_COLLECTED_COOKIES_MULTIPLE_BLOCK_RULES_CREATED);

      case CONTENT_SETTING_ALLOW:
        return l10n_util::GetStringUTF8(
            IDS_COLLECTED_COOKIES_MULTIPLE_ALLOW_RULES_CREATED);

      case CONTENT_SETTING_SESSION_ONLY:
        return l10n_util::GetStringUTF8(
            IDS_COLLECTED_COOKIES_MULTIPLE_SESSION_RULES_CREATED);

      default:
        NOTREACHED();
        return std::string();
    }
  }

  switch (setting) {
    case CONTENT_SETTING_BLOCK:
      return l10n_util::GetStringFUTF8(
          IDS_COLLECTED_COOKIES_BLOCK_RULE_CREATED, domain_name);

    case CONTENT_SETTING_ALLOW:
      return l10n_util::GetStringFUTF8(
          IDS_COLLECTED_COOKIES_ALLOW_RULE_CREATED, domain_name);

    case CONTENT_SETTING_SESSION_ONLY:
      return l10n_util::GetStringFUTF8(
          IDS_COLLECTED_COOKIES_SESSION_RULE_CREATED, domain_name);

    default:
      NOTREACHED();
      return std::string();
  }
}
}  // namespace

CollectedCookiesGtk::CollectedCookiesGtk(GtkWindow* parent,
                                         TabContents* tab_contents)
    : tab_contents_(tab_contents) {
  TabSpecificContentSettings* content_settings =
      tab_contents->GetTabSpecificContentSettings();
  registrar_.Add(this, NotificationType::COLLECTED_COOKIES_SHOWN,
                 Source<TabSpecificContentSettings>(content_settings));

  Init();
}

void CollectedCookiesGtk::Init() {
  HostContentSettingsMap* host_content_settings_map =
      tab_contents_->profile()->GetHostContentSettingsMap();

  dialog_ = gtk_vbox_new(FALSE, gtk_util::kContentAreaSpacing);
  gtk_box_set_spacing(GTK_BOX(dialog_), gtk_util::kContentAreaSpacing);

  GtkWidget* label = gtk_label_new(
      l10n_util::GetStringUTF8(IDS_COLLECTED_COOKIES_DIALOG_TITLE).c_str());
  gtk_box_pack_start(GTK_BOX(dialog_), label, TRUE, TRUE, 0);

  // Allowed Cookie list.
  GtkWidget* cookie_list_vbox = gtk_vbox_new(FALSE, gtk_util::kControlSpacing);
  gtk_box_pack_start(GTK_BOX(dialog_), cookie_list_vbox, TRUE, TRUE, 0);

  label = gtk_label_new(
      l10n_util::GetStringUTF8(IDS_COLLECTED_COOKIES_ALLOWED_COOKIES_LABEL).
          c_str());
  gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
  gtk_box_pack_start(GTK_BOX(cookie_list_vbox), label, FALSE, FALSE, 0);

  GtkWidget* scroll_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_window),
                                 GTK_POLICY_AUTOMATIC,
                                 GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll_window),
                                      GTK_SHADOW_ETCHED_IN);
  gtk_box_pack_start(GTK_BOX(cookie_list_vbox), scroll_window, TRUE, TRUE, 0);

  TabSpecificContentSettings* content_settings =
      tab_contents_->GetTabSpecificContentSettings();

  allowed_cookies_tree_model_.reset(
      content_settings->GetAllowedCookiesTreeModel());
  allowed_cookies_tree_adapter_.reset(
      new gtk_tree::TreeAdapter(this, allowed_cookies_tree_model_.get()));
  allowed_tree_ = gtk_tree_view_new_with_model(
      GTK_TREE_MODEL(allowed_cookies_tree_adapter_->tree_store()));
  gtk_widget_set_size_request(allowed_tree_, kTreeViewWidth, kTreeViewHeight);
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(allowed_tree_), FALSE);
  gtk_tree_view_set_enable_tree_lines(GTK_TREE_VIEW(allowed_tree_), TRUE);
  gtk_container_add(GTK_CONTAINER(scroll_window), allowed_tree_);

  GtkTreeViewColumn* title_column = gtk_tree_view_column_new();
  GtkCellRenderer* pixbuf_renderer = gtk_cell_renderer_pixbuf_new();
  gtk_tree_view_column_pack_start(title_column, pixbuf_renderer, FALSE);
  gtk_tree_view_column_add_attribute(title_column, pixbuf_renderer, "pixbuf",
                                     gtk_tree::TreeAdapter::COL_ICON);
  GtkCellRenderer* title_renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(title_column, title_renderer, TRUE);
  gtk_tree_view_column_add_attribute(title_column, title_renderer, "text",
                                     gtk_tree::TreeAdapter::COL_TITLE);
  gtk_tree_view_column_set_title(
      title_column, l10n_util::GetStringUTF8(
          IDS_COOKIES_DOMAIN_COLUMN_HEADER).c_str());
  gtk_tree_view_append_column(GTK_TREE_VIEW(allowed_tree_), title_column);
  g_signal_connect(allowed_tree_, "row-expanded",
                   G_CALLBACK(OnTreeViewRowExpandedThunk), this);
  allowed_selection_ =
      gtk_tree_view_get_selection(GTK_TREE_VIEW(allowed_tree_));
  gtk_tree_selection_set_mode(allowed_selection_, GTK_SELECTION_MULTIPLE);
  g_signal_connect(allowed_selection_, "changed",
                   G_CALLBACK(OnTreeViewSelectionChangeThunk), this);

  GtkWidget* button_box = gtk_hbutton_box_new();
  gtk_button_box_set_layout(GTK_BUTTON_BOX(button_box), GTK_BUTTONBOX_START);
  gtk_box_set_spacing(GTK_BOX(button_box), gtk_util::kControlSpacing);
  gtk_box_pack_start(GTK_BOX(dialog_), button_box, FALSE, FALSE, 0);
  block_allowed_cookie_button_ = gtk_button_new_with_label(
      l10n_util::GetStringUTF8(IDS_COLLECTED_COOKIES_BLOCK_BUTTON).c_str());
  g_signal_connect(block_allowed_cookie_button_, "clicked",
                   G_CALLBACK(OnBlockAllowedButtonClickedThunk), this);
  gtk_container_add(GTK_CONTAINER(button_box), block_allowed_cookie_button_);

  GtkWidget* separator = gtk_hseparator_new();
  gtk_box_pack_start(GTK_BOX(dialog_), separator, TRUE, TRUE, 0);

  // Blocked Cookie list.
  cookie_list_vbox = gtk_vbox_new(FALSE, gtk_util::kControlSpacing);
  gtk_box_pack_start(GTK_BOX(dialog_), cookie_list_vbox, TRUE, TRUE, 0);

  label = gtk_label_new(
      l10n_util::GetStringUTF8(
          host_content_settings_map->BlockThirdPartyCookies() ?
              IDS_COLLECTED_COOKIES_BLOCKED_THIRD_PARTY_BLOCKING_ENABLED :
              IDS_COLLECTED_COOKIES_BLOCKED_COOKIES_LABEL).c_str());
  gtk_widget_set_size_request(label, kTreeViewWidth, -1);
  gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
  gtk_misc_set_alignment(GTK_MISC(label), 0, 0.5);
  gtk_box_pack_start(GTK_BOX(cookie_list_vbox), label, TRUE, TRUE, 0);

  scroll_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_window),
                                 GTK_POLICY_AUTOMATIC,
                                 GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll_window),
                                      GTK_SHADOW_ETCHED_IN);
  gtk_box_pack_start(GTK_BOX(cookie_list_vbox), scroll_window, TRUE, TRUE, 0);

  blocked_cookies_tree_model_.reset(
      content_settings->GetBlockedCookiesTreeModel());
  blocked_cookies_tree_adapter_.reset(
      new gtk_tree::TreeAdapter(this, blocked_cookies_tree_model_.get()));
  blocked_tree_ = gtk_tree_view_new_with_model(
      GTK_TREE_MODEL(blocked_cookies_tree_adapter_->tree_store()));
  gtk_widget_set_size_request(blocked_tree_, kTreeViewWidth, kTreeViewHeight);
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(blocked_tree_), FALSE);
  gtk_tree_view_set_enable_tree_lines(GTK_TREE_VIEW(blocked_tree_), TRUE);
  gtk_container_add(GTK_CONTAINER(scroll_window), blocked_tree_);

  title_column = gtk_tree_view_column_new();
  pixbuf_renderer = gtk_cell_renderer_pixbuf_new();
  gtk_tree_view_column_pack_start(title_column, pixbuf_renderer, FALSE);
  gtk_tree_view_column_add_attribute(title_column, pixbuf_renderer, "pixbuf",
                                     gtk_tree::TreeAdapter::COL_ICON);
  title_renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(title_column, title_renderer, TRUE);
  gtk_tree_view_column_add_attribute(title_column, title_renderer, "text",
                                     gtk_tree::TreeAdapter::COL_TITLE);
  gtk_tree_view_column_set_title(
      title_column, l10n_util::GetStringUTF8(
          IDS_COOKIES_DOMAIN_COLUMN_HEADER).c_str());
  gtk_tree_view_append_column(GTK_TREE_VIEW(blocked_tree_), title_column);
  g_signal_connect(blocked_tree_, "row-expanded",
                   G_CALLBACK(OnTreeViewRowExpandedThunk), this);
  blocked_selection_ =
      gtk_tree_view_get_selection(GTK_TREE_VIEW(blocked_tree_));
  gtk_tree_selection_set_mode(blocked_selection_, GTK_SELECTION_MULTIPLE);
  g_signal_connect(blocked_selection_, "changed",
                   G_CALLBACK(OnTreeViewSelectionChangeThunk), this);

  button_box = gtk_hbutton_box_new();
  gtk_button_box_set_layout(GTK_BUTTON_BOX(button_box), GTK_BUTTONBOX_START);
  gtk_box_set_spacing(GTK_BOX(button_box), gtk_util::kControlSpacing);
  gtk_box_pack_start(GTK_BOX(dialog_), button_box, FALSE, FALSE, 0);
  allow_blocked_cookie_button_ = gtk_button_new_with_label(
      l10n_util::GetStringUTF8(IDS_COLLECTED_COOKIES_ALLOW_BUTTON).c_str());
  g_signal_connect(allow_blocked_cookie_button_, "clicked",
                   G_CALLBACK(OnAllowBlockedButtonClickedThunk), this);
  gtk_container_add(GTK_CONTAINER(button_box), allow_blocked_cookie_button_);
  for_session_blocked_cookie_button_ = gtk_button_new_with_label(
      l10n_util::GetStringUTF8(IDS_COLLECTED_COOKIES_SESSION_ONLY_BUTTON).
          c_str());
  g_signal_connect(for_session_blocked_cookie_button_, "clicked",
                   G_CALLBACK(OnForSessionBlockedButtonClickedThunk), this);
  gtk_container_add(GTK_CONTAINER(button_box),
                    for_session_blocked_cookie_button_);

  // Infobar.
  infobar_ = gtk_frame_new(NULL);
  GtkWidget* infobar_contents = gtk_hbox_new(FALSE, kBannerPadding);
  gtk_container_set_border_width(GTK_CONTAINER(infobar_contents),
                                 kBannerPadding);
  gtk_container_add(GTK_CONTAINER(infobar_), infobar_contents);
  GtkWidget* info_image =
      gtk_image_new_from_stock(GTK_STOCK_DIALOG_INFO,
                               GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_box_pack_start(GTK_BOX(infobar_contents), info_image, FALSE, FALSE, 0);
  infobar_label_ = gtk_label_new(NULL);
  gtk_box_pack_start(
      GTK_BOX(infobar_contents), infobar_label_, FALSE, FALSE, 0);
  gtk_widget_show_all(infobar_);
  gtk_widget_set_no_show_all(infobar_, TRUE);
  gtk_widget_hide(infobar_);
  gtk_box_pack_start(GTK_BOX(dialog_), infobar_, TRUE, TRUE, 0);

  // Close button.
  button_box = gtk_hbutton_box_new();
  gtk_button_box_set_layout(GTK_BUTTON_BOX(button_box), GTK_BUTTONBOX_END);
  gtk_box_set_spacing(GTK_BOX(button_box), gtk_util::kControlSpacing);
  gtk_box_pack_end(GTK_BOX(dialog_), button_box, FALSE, TRUE, 0);
  GtkWidget* close = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
  gtk_button_set_label(GTK_BUTTON(close),
                       l10n_util::GetStringUTF8(IDS_CLOSE).c_str());
  g_signal_connect(close, "clicked", G_CALLBACK(OnCloseThunk), this);
  gtk_box_pack_end(GTK_BOX(button_box), close, FALSE, TRUE, 0);

  // Show the dialog.
  allowed_cookies_tree_adapter_->Init();
  blocked_cookies_tree_adapter_->Init();
  EnableControls();
  window_ = tab_contents_->CreateConstrainedDialog(this);
}

CollectedCookiesGtk::~CollectedCookiesGtk() {
  gtk_widget_destroy(dialog_);
}

GtkWidget* CollectedCookiesGtk::GetWidgetRoot() {
  return dialog_;
}

void CollectedCookiesGtk::DeleteDelegate() {
  delete this;
}

bool CollectedCookiesGtk::SelectionContainsOriginNode(
    GtkTreeSelection* selection, gtk_tree::TreeAdapter* adapter) {
  // Check whether at least one "origin" node is selected.
  GtkTreeModel* model;
  GList* paths =
      gtk_tree_selection_get_selected_rows(selection, &model);
  bool contains_origin_node = false;
  for (GList* item = paths; item; item = item->next) {
    GtkTreeIter iter;
    gtk_tree_model_get_iter(
        model, &iter, reinterpret_cast<GtkTreePath*>(item->data));
    CookieTreeNode* node =
        static_cast<CookieTreeNode*>(adapter->GetNode(&iter));
    if (node->GetDetailedInfo().node_type !=
        CookieTreeNode::DetailedInfo::TYPE_ORIGIN)
      continue;
    CookieTreeOriginNode* origin_node = static_cast<CookieTreeOriginNode*>(
        node);
    if (!origin_node->CanCreateContentException())
      continue;
    contains_origin_node = true;
  }
  g_list_foreach(paths, reinterpret_cast<GFunc>(gtk_tree_path_free), NULL);
  g_list_free(paths);
  return contains_origin_node;
}

void CollectedCookiesGtk::EnableControls() {
  // Update button states.
  bool enable_for_allowed_cookies =
      SelectionContainsOriginNode(allowed_selection_,
                                  allowed_cookies_tree_adapter_.get());
  gtk_widget_set_sensitive(block_allowed_cookie_button_,
                           enable_for_allowed_cookies);

  bool enable_for_blocked_cookies =
      SelectionContainsOriginNode(blocked_selection_,
                                  blocked_cookies_tree_adapter_.get());
  gtk_widget_set_sensitive(allow_blocked_cookie_button_,
                           enable_for_blocked_cookies);
  gtk_widget_set_sensitive(for_session_blocked_cookie_button_,
                           enable_for_blocked_cookies);
}

void CollectedCookiesGtk::Observe(NotificationType type,
                                  const NotificationSource& source,
                                  const NotificationDetails& details) {
  DCHECK(type == NotificationType::COLLECTED_COOKIES_SHOWN);
  DCHECK_EQ(Source<TabSpecificContentSettings>(source).ptr(),
            tab_contents_->GetTabSpecificContentSettings());
  window_->CloseConstrainedWindow();
}

void CollectedCookiesGtk::OnClose(GtkWidget* close_button) {
  window_->CloseConstrainedWindow();
}

void CollectedCookiesGtk::AddExceptions(GtkTreeSelection* selection,
                                        gtk_tree::TreeAdapter* adapter,
                                        ContentSetting setting) {
  GtkTreeModel* model;
  GList* paths =
      gtk_tree_selection_get_selected_rows(selection, &model);
  string16 last_domain_name;
  bool multiple_domains_added = false;
  for (GList* item = paths; item; item = item->next) {
    GtkTreeIter iter;
    gtk_tree_model_get_iter(
        model, &iter, reinterpret_cast<GtkTreePath*>(item->data));
    CookieTreeNode* node =
        static_cast<CookieTreeNode*>(adapter->GetNode(&iter));
    if (node->GetDetailedInfo().node_type !=
        CookieTreeNode::DetailedInfo::TYPE_ORIGIN)
      continue;
    CookieTreeOriginNode* origin_node = static_cast<CookieTreeOriginNode*>(
        node);
    if (origin_node->CanCreateContentException()) {
      if (!last_domain_name.empty())
        multiple_domains_added = true;
      last_domain_name = origin_node->GetTitle();
      origin_node->CreateContentException(
          tab_contents_->profile()->GetHostContentSettingsMap(), setting);
    }
  }
  g_list_foreach(paths, reinterpret_cast<GFunc>(gtk_tree_path_free), NULL);
  g_list_free(paths);
  if (last_domain_name.empty()) {
    gtk_widget_hide(infobar_);
  } else {
    gtk_label_set_text(
        GTK_LABEL(infobar_label_), GetInfobarLabel(
            setting, multiple_domains_added, last_domain_name).c_str());
    gtk_widget_show(infobar_);
  }
}

void CollectedCookiesGtk::OnBlockAllowedButtonClicked(GtkWidget* button) {
  AddExceptions(allowed_selection_, allowed_cookies_tree_adapter_.get(),
                CONTENT_SETTING_BLOCK);
}

void CollectedCookiesGtk::OnAllowBlockedButtonClicked(GtkWidget* button) {
  AddExceptions(blocked_selection_, blocked_cookies_tree_adapter_.get(),
                CONTENT_SETTING_ALLOW);
}

void CollectedCookiesGtk::OnForSessionBlockedButtonClicked(GtkWidget* button) {
  AddExceptions(blocked_selection_, blocked_cookies_tree_adapter_.get(),
                CONTENT_SETTING_SESSION_ONLY);
}

void CollectedCookiesGtk::OnTreeViewRowExpanded(GtkWidget* tree_view,
                                        GtkTreeIter* iter,
                                        GtkTreePath* path) {
  // When a row in the tree is expanded, expand all the children too.
  g_signal_handlers_block_by_func(
      tree_view, reinterpret_cast<gpointer>(OnTreeViewRowExpandedThunk), this);
  gtk_tree_view_expand_row(GTK_TREE_VIEW(tree_view), path, TRUE);
  g_signal_handlers_unblock_by_func(
      tree_view, reinterpret_cast<gpointer>(OnTreeViewRowExpandedThunk), this);
}

void CollectedCookiesGtk::OnTreeViewSelectionChange(GtkWidget* selection) {
  EnableControls();
}
