// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Implements a basic file I/O URLProtocol for FFmpeg.  Since we don't build
// FFmpeg binaries with protocols, we have to write our own.

#ifndef MEDIA_FFMPEG_FILE_PROTOCOL_H_
#define MEDIA_FFMPEG_FILE_PROTOCOL_H_

struct URLProtocol;
extern URLProtocol kFFmpegFileProtocol;

#endif  // MEDIA_FFMPEG_FILE_PROTOCOL_H_
