// Copyright (c) 2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VIEWS_EXAMPLES_MENU_EXAMPLE_H_
#define VIEWS_EXAMPLES_MENU_EXAMPLE_H_
#pragma once

#include <set>

#include "app/menus/simple_menu_model.h"
#include "base/utf_string_conversions.h"
#include "views/controls/button/menu_button.h"
#include "views/controls/menu/menu_2.h"
#include "views/controls/menu/view_menu_delegate.h"
#include "views/controls/button/text_button.h"
#include "views/examples/example_base.h"
#include "views/fill_layout.h"

namespace examples {

class ExampleMenuContents : public menus::SimpleMenuModel,
                            public menus::SimpleMenuModel::Delegate {
  enum {
    kGroupMakeDecision,
  };

  enum {
    kCommandDoSomething,
    kCommandSelectAscii,
    kCommandSelectUtf8,
    kCommandSelectUtf16,
    kCommandCheckApple,
    kCommandCheckOrange,
    kCommandCheckKiwi,
    kCommandGoHome,
  };

 public:
  ExampleMenuContents() :
      ALLOW_THIS_IN_INITIALIZER_LIST(menus::SimpleMenuModel(this)),
      current_encoding_command_id_(kCommandSelectAscii) {

    AddItem(kCommandDoSomething, WideToUTF16(L"Do Something"));
    AddSeparator();
    AddRadioItem(kCommandSelectAscii,
                 WideToUTF16(L"ASCII"), kGroupMakeDecision);
    AddRadioItem(kCommandSelectUtf8,
                 WideToUTF16(L"UTF-8"), kGroupMakeDecision);
    AddRadioItem(kCommandSelectUtf16,
                 WideToUTF16(L"UTF-16"), kGroupMakeDecision);
    AddSeparator();
    AddCheckItem(kCommandCheckApple, WideToUTF16(L"Apple"));
    AddCheckItem(kCommandCheckOrange, WideToUTF16(L"Orange"));
    AddCheckItem(kCommandCheckKiwi, WideToUTF16(L"Kiwi"));
    AddSeparator();
    AddItem(kCommandGoHome, WideToUTF16(L"Go Home"));

    submenu_.reset(new menus::SimpleMenuModel(this));
    submenu_->AddItem(kCommandDoSomething, WideToUTF16(L"Do Something 2"));
    AddSubMenu(-1, ASCIIToUTF16("Submenu"), submenu_.get());
    menu_.reset(new views::Menu2(this));
  }

  void RunMenuAt(const gfx::Point& point) {
    menu_->RunMenuAt(point, views::Menu2::ALIGN_TOPRIGHT);
  }

  // menus::SimpleMenuModel::Delegate implementation.
  virtual bool IsCommandIdChecked(int command_id) const {
    // Radio items.
    if (command_id == current_encoding_command_id_) {
      return true;
    }

    // Check items.
    if (checked_fruits_.find(command_id) != checked_fruits_.end()) {
      return true;
    }
    return false;
  }

  virtual bool IsCommandIdEnabled(int command_id) const {
    // All commands are enabled except for kCommandGoHome.
    return command_id != kCommandGoHome;
  }

  virtual bool GetAcceleratorForCommandId(
      int command_id,
      menus::Accelerator* accelerator) {
    // We don't use this in the example.
    return false;
  }

  virtual void ExecuteCommand(int command_id) {
    switch (command_id) {
      case kCommandDoSomething: {
        LOG(INFO) << "Done something";
        break;
      }

      // Radio items.
      case kCommandSelectAscii: {
        current_encoding_command_id_ = kCommandSelectAscii;
        LOG(INFO) << "Selected ASCII";
        break;
      }
      case kCommandSelectUtf8: {
        current_encoding_command_id_ = kCommandSelectUtf8;
        LOG(INFO) << "Selected UTF-8";
        break;
      }
      case kCommandSelectUtf16: {
        current_encoding_command_id_ = kCommandSelectUtf16;
        LOG(INFO) << "Selected UTF-16";
        break;
      }

      // Check items.
      case kCommandCheckApple:
      case kCommandCheckOrange:
      case kCommandCheckKiwi: {
        // Print what fruit is checked.
        const char* checked_fruit = "";
        if (command_id == kCommandCheckApple) {
          checked_fruit = "Apple";
        } else if (command_id == kCommandCheckOrange) {
          checked_fruit = "Orange";
        } else if (command_id == kCommandCheckKiwi) {
          checked_fruit = "Kiwi";
        }
        LOG(INFO) << "Checked " << checked_fruit;

        // Update the check status.
        std::set<int>::iterator iter = checked_fruits_.find(command_id);
        if (iter == checked_fruits_.end()) {
          checked_fruits_.insert(command_id);
        } else {
          checked_fruits_.erase(iter);
        }
        break;
      }
    }
  }

 private:
  scoped_ptr<views::Menu2> menu_;
  scoped_ptr<menus::SimpleMenuModel> submenu_;
  std::set<int> checked_fruits_;
  int current_encoding_command_id_;

  DISALLOW_COPY_AND_ASSIGN(ExampleMenuContents);
};

class ExampleMenuButton : public views::MenuButton,
                          public views::ViewMenuDelegate {
 public:
  ExampleMenuButton(const std::wstring& test,
                    bool show_menu_marker)
      : ALLOW_THIS_IN_INITIALIZER_LIST(
          views::MenuButton(NULL, test, this, show_menu_marker)) {
  }

 private:
  // views::ViewMenuDelegate implementation.
  virtual void RunMenu(views::View* source, const gfx::Point& point) {
    if (menu_contents_ == NULL) {
      menu_contents_.reset(new ExampleMenuContents);
    }
    menu_contents_->RunMenuAt(point);
  }

  scoped_ptr<ExampleMenuContents> menu_contents_;
  DISALLOW_COPY_AND_ASSIGN(ExampleMenuButton);
};

// MenuExample demonstrates how to use the Menu class.
class MenuExample : public ExampleBase {
 public:
  explicit MenuExample(ExamplesMain* main)
      : ExampleBase(main) {
  }

  virtual ~MenuExample() {}

  virtual std::wstring GetExampleTitle() {
    return L"Menu";
  }

  virtual void CreateExampleView(views::View* container) {
    // views::Menu2 is not a sub class of View, hence we cannot directly
    // add to the continer. Instead, we add a button to open a menu.
    const bool show_menu_marker = true;
    ExampleMenuButton* menu_button = new ExampleMenuButton(
        L"Open a menu", show_menu_marker);
    container->SetLayoutManager(new views::FillLayout);
    container->AddChildView(menu_button);
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(MenuExample);
};

}  // namespace examples

#endif  // VIEWS_EXAMPLES_MENU_EXAMPLE_H_
