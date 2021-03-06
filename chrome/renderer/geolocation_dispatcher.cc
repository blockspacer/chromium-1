// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(ENABLE_CLIENT_BASED_GEOLOCATION)

#include "chrome/renderer/geolocation_dispatcher.h"

#include "chrome/renderer/render_view.h"
#include "ipc/ipc_message.h"
#include "third_party/WebKit/WebKit/chromium/public/WebGeolocationPermissionRequest.h"
#include "third_party/WebKit/WebKit/chromium/public/WebGeolocationPermissionRequestManager.h"
#include "third_party/WebKit/WebKit/chromium/public/WebGeolocationClient.h"
#include "third_party/WebKit/WebKit/chromium/public/WebGeolocationPosition.h"
#include "third_party/WebKit/WebKit/chromium/public/WebGeolocationError.h"
#include "third_party/WebKit/WebKit/chromium/public/WebSecurityOrigin.h"

using namespace WebKit;

GeolocationDispatcher::GeolocationDispatcher(RenderView* render_view)
    : render_view_(render_view),
      pending_permissions_(new WebGeolocationPermissionRequestManager()),
      enable_high_accuracy_(false),
      updating_(false) {
}

GeolocationDispatcher::~GeolocationDispatcher() {}

bool GeolocationDispatcher::OnMessageReceived(const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(GeolocationDispatcher, message)
    IPC_MESSAGE_HANDLER(ViewMsg_Geolocation_PermissionSet,
                        OnGeolocationPermissionSet)
    IPC_MESSAGE_HANDLER(ViewMsg_Geolocation_PositionUpdated,
                        OnGeolocationPositionUpdated)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  return handled;
}

void GeolocationDispatcher::geolocationDestroyed() {
  controller_.reset();
  DCHECK(!updating_);
}

void GeolocationDispatcher::startUpdating() {
  // TODO(jknotten): Remove url and bridge_id from StartUpdating message
  // once we have switched over to client-based geolocation.
  GURL url;
  render_view_->Send(new ViewHostMsg_Geolocation_StartUpdating(
      render_view_->routing_id(), -1, url, enable_high_accuracy_));
  updating_ = true;
}

void GeolocationDispatcher::stopUpdating() {
  // TODO(jknotten): Remove url and bridge_id from StopUpdating message
  // once we have switched over to client-based geolocation.
  render_view_->Send(new ViewHostMsg_Geolocation_StopUpdating(
      render_view_->routing_id(), -1));
  updating_ = false;
}

void GeolocationDispatcher::setEnableHighAccuracy(bool enable_high_accuracy) {
  // GeolocationController calls setEnableHighAccuracy(true) before
  // startUpdating in response to the first high-accuracy Geolocation
  // subscription. When the last high-accuracy Geolocation unsubscribes
  // it calls setEnableHighAccuracy(false) after stopUpdating.
  bool has_changed = enable_high_accuracy_ != enable_high_accuracy;
  enable_high_accuracy_ = enable_high_accuracy;
  // We have a different accuracy requirement. Request browser to update.
  if (updating_ && has_changed)
    startUpdating();
}

void GeolocationDispatcher::setController(
    WebGeolocationController* controller) {
  controller_.reset(controller);
}

bool GeolocationDispatcher::lastPosition(WebGeolocationPosition&) {
  // The latest position is stored in the browser, not the renderer, so we
  // would have to fetch it synchronously to give a good value here.  The
  // WebCore::GeolocationController already caches the last position it
  // receives, so there is not much benefit to more position caching here.
  return false;
}

// TODO(jknotten): Change the messages to use a security origin, so no
// conversion is necessary.
void GeolocationDispatcher::requestPermission(
    const WebGeolocationPermissionRequest& permissionRequest) {
  int bridge_id = pending_permissions_->add(permissionRequest);
  string16 origin = permissionRequest.securityOrigin().toString();
  render_view_->Send(new ViewHostMsg_Geolocation_RequestPermission(
      render_view_->routing_id(), bridge_id, GURL(origin)));
}

// TODO(jknotten): Change the messages to use a security origin, so no
// conversion is necessary.
void GeolocationDispatcher::cancelPermissionRequest(
    const WebGeolocationPermissionRequest& permissionRequest) {
  int bridge_id;
  if (!pending_permissions_->remove(permissionRequest, bridge_id))
    return;
  string16 origin = permissionRequest.securityOrigin().toString();
  render_view_->Send(new ViewHostMsg_Geolocation_CancelPermissionRequest(
      render_view_->routing_id(), bridge_id, GURL(origin)));
}

// Permission for using geolocation has been set.
void GeolocationDispatcher::OnGeolocationPermissionSet(
    int bridge_id, bool is_allowed) {
  WebGeolocationPermissionRequest permissionRequest;
  if (!pending_permissions_->remove(bridge_id, permissionRequest))
    return;
  permissionRequest.setIsAllowed(is_allowed);
}

// We have an updated geolocation position or error code.
void GeolocationDispatcher::OnGeolocationPositionUpdated(
    const Geoposition& geoposition) {
  DCHECK(updating_);
  DCHECK(geoposition.IsInitialized());
  if (geoposition.IsValidFix()) {
    controller_->positionChanged(
        WebGeolocationPosition(
            geoposition.timestamp.ToDoubleT() * 1000.0,
            geoposition.latitude, geoposition.longitude,
            geoposition.accuracy,
            geoposition.is_valid_altitude(), geoposition.altitude,
            geoposition.is_valid_altitude_accuracy(),
            geoposition.altitude_accuracy,
            geoposition.is_valid_heading(), geoposition.heading,
            geoposition.is_valid_speed(), geoposition.speed));
  } else {
    WebGeolocationError::Error code;
    switch (geoposition.error_code) {
      case Geoposition::ERROR_CODE_PERMISSION_DENIED:
        code = WebGeolocationError::ErrorPermissionDenied;
        break;
      case Geoposition::ERROR_CODE_POSITION_UNAVAILABLE:
        code = WebGeolocationError::ErrorPositionUnavailable;
        break;
      default:
        DCHECK(false);
        NOTREACHED() << geoposition.error_code;
        return;
    }
    controller_->errorOccurred(
        WebGeolocationError(
            code, WebKit::WebString::fromUTF8(geoposition.error_message)));
  }
}

#endif // CLIENT_BASED_GEOLOCATION
