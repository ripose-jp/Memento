/*
 * Copyright (c) 2013-2018 Andreas Unterweger
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef TRANSCODE_AAC_H
#define TRANSCODE_AAC_H

#include <stdio.h>

/**
 * Clip the audio from an input file to an AAC output file.
 * @param input_file    The file to read audio streams from.
 * @param output_file   The output file.
 * @param audio_track   Index of the audio stream to clip. Starts from 0.
 * @param start         Start time of the clip.
 * @param end           End time of the clip.
 * @return Error code (0 if successful)
 */
int transcode_aac(const char *input_file, const char *output_file,
                  const size_t audio_track,
                  const double start, const double end);

#endif // TRANSCODE_AAC_H