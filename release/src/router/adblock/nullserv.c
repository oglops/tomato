/*
A minimal webserver for serving up null content.
Copyright (c) 2013 Flexion.Org, http://flexion.org.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
//#include <syslog.h>

static const unsigned char null_gif[] =
{
    0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x01, 0x00, 0x01, 0x00, 0xf0, 0x00, 0x00, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x21, 0xf9, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x00, 0x02, 0x02, 0x44, 0x01, 0x00, 0x3b
};

static const unsigned char null_png[] =
{
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x00, 0x00, 0x00, 0x25, 0xdb, 0x56,
    0xca, 0x00, 0x00, 0x00, 0x03, 0x73, 0x42, 0x49, 0x54, 0x08, 0x08, 0x08, 0xdb, 0xe1, 0x4f, 0xe0,
    0x00, 0x00, 0x00, 0x06, 0x50, 0x4c, 0x54, 0x45, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x55, 0xc2,
    0xd3, 0x7e, 0x00, 0x00, 0x00, 0x02, 0x74, 0x52, 0x4e, 0x53, 0x00, 0xff, 0x5b, 0x91, 0x22, 0xb5,
    0x00, 0x00, 0x00, 0x0a, 0x49, 0x44, 0x41, 0x54, 0x08, 0x99, 0x63, 0x60, 0x00, 0x00, 0x00, 0x02,
    0x00, 0x01, 0xf4, 0x71, 0x64, 0xa6, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42,
    0x60, 0x82
};

static const unsigned char null_jpg[] =
{
    0xff, 0xd8, 0xff, 0xdb, 0x00, 0x43, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xff, 0xc0, 0x00, 0x0b, 0x08, 0x00, 0x01, 0x00, 0x01,
    0x01, 0x01, 0x11, 0x00, 0xff, 0xc4, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0xff, 0xc4, 0x00, 0x14, 0x10, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xda, 0x00, 0x08, 0x01, 0x01, 0x00, 0x00, 0x3f, 0x00, 0x7f, 0x0f, 0xff, 0xd9
};

static const unsigned char null_swf[] =
{
    0x88, 0x08, 0x02, 0x35, 0x00, 0x42, 0x75, 0x66, 0x66, 0x65, 0x72, 0x00, 0x4E, 0x61, 0x74, 0x69,
    0x76, 0x65, 0x00, 0x6C, 0x6F, 0x61, 0x64, 0x00, 0x42, 0x75, 0x66, 0x66, 0x65, 0x72, 0x5F, 0x6C,
    0x6F, 0x61, 0x64, 0x00, 0x70, 0x72, 0x6F, 0x74, 0x6F, 0x74, 0x79, 0x70, 0x65, 0x00, 0x64, 0x69,
    0x66, 0x66, 0x00, 0x42, 0x75, 0x66, 0x66, 0x65, 0x72, 0x5F, 0x64, 0x69, 0x66, 0x66, 0x00, 0x66,
    0x69, 0x6E, 0x64, 0x00, 0x42, 0x75, 0x66, 0x66, 0x65, 0x72, 0x5F, 0x66, 0x69, 0x6E, 0x64, 0x00,
    0x73, 0x75, 0x62, 0x00, 0x42, 0x75, 0x66, 0x66, 0x65, 0x72, 0x5F, 0x73, 0x75, 0x62, 0x00, 0x74,
    0x6F, 0x53, 0x74, 0x72, 0x69, 0x6E, 0x67, 0x00, 0x42, 0x75, 0x66, 0x66, 0x65, 0x72, 0x5F, 0x74,
    0x6F, 0x53, 0x74, 0x72, 0x69, 0x6E, 0x67, 0x00, 0x49, 0x6D, 0x61, 0x67, 0x65, 0x00, 0x63, 0x6F,
    0x6D, 0x70, 0x61, 0x72, 0x65, 0x00, 0x49, 0x6D, 0x61, 0x67, 0x65, 0x5F, 0x63, 0x6F, 0x6D, 0x70,
    0x61, 0x72, 0x65, 0x00, 0x73, 0x61, 0x76, 0x65, 0x00, 0x49, 0x6D, 0x61, 0x67, 0x65, 0x5F, 0x73,
    0x61, 0x76, 0x65, 0x00, 0x53, 0x6F, 0x63, 0x6B, 0x65, 0x74, 0x00, 0x63, 0x6C, 0x6F, 0x73, 0x65,
    0x00, 0x53, 0x6F, 0x63, 0x6B, 0x65, 0x74, 0x5F, 0x63, 0x6C, 0x6F, 0x73, 0x65, 0x00, 0x65, 0x72,
    0x72, 0x6F, 0x72, 0x00, 0x53, 0x6F, 0x63, 0x6B, 0x65, 0x74, 0x5F, 0x65, 0x72, 0x72, 0x6F, 0x72,
    0x00, 0x73, 0x65, 0x6E, 0x64, 0x00, 0x53, 0x6F, 0x63, 0x6B, 0x65, 0x74, 0x5F, 0x73, 0x65, 0x6E,
    0x64, 0x00, 0x63, 0x6C, 0x6F, 0x73, 0x65, 0x64, 0x00, 0x53, 0x6F, 0x63, 0x6B, 0x65, 0x74, 0x5F,
    0x67, 0x65, 0x74, 0x5F, 0x63, 0x6C, 0x6F, 0x73, 0x65, 0x64, 0x00, 0x61, 0x64, 0x64, 0x50, 0x72,
    0x6F, 0x70, 0x65, 0x72, 0x74, 0x79, 0x00, 0x54, 0x65, 0x73, 0x74, 0x00, 0x61, 0x64, 0x76, 0x61,
    0x6E, 0x63, 0x65, 0x00, 0x54, 0x65, 0x73, 0x74, 0x5F, 0x61, 0x64, 0x76, 0x61, 0x6E, 0x63, 0x65,
    0x00, 0x6D, 0x6F, 0x75, 0x73, 0x65, 0x5F, 0x6D, 0x6F, 0x76, 0x65, 0x00, 0x54, 0x65, 0x73, 0x74,
    0x5F, 0x6D, 0x6F, 0x75, 0x73, 0x65, 0x5F, 0x6D, 0x6F, 0x76, 0x65, 0x00, 0x6D, 0x6F, 0x75, 0x73,
    0x65, 0x5F, 0x70, 0x72, 0x65, 0x73, 0x73, 0x00, 0x54, 0x65, 0x73, 0x74, 0x5F, 0x6D, 0x6F, 0x75,
    0x73, 0x65, 0x5F, 0x70, 0x72, 0x65, 0x73, 0x73, 0x00, 0x6D, 0x6F, 0x75, 0x73, 0x65, 0x5F, 0x72,
    0x65, 0x6C, 0x65, 0x61, 0x73, 0x65, 0x00, 0x54, 0x65, 0x73, 0x74, 0x5F, 0x6D, 0x6F, 0x75, 0x73,
    0x65, 0x5F, 0x72, 0x65, 0x6C, 0x65, 0x61, 0x73, 0x65, 0x00, 0x72, 0x65, 0x6E, 0x64, 0x65, 0x72,
    0x00, 0x54, 0x65, 0x73, 0x74, 0x5F, 0x72, 0x65, 0x6E, 0x64, 0x65, 0x72, 0x00, 0x72, 0x65, 0x73,
    0x65, 0x74, 0x00, 0x54, 0x65, 0x73, 0x74, 0x5F, 0x72, 0x65, 0x73, 0x65, 0x74, 0x00, 0x72, 0x61,
    0x74, 0x65, 0x00, 0x54, 0x65, 0x73, 0x74, 0x5F, 0x67, 0x65, 0x74, 0x5F, 0x72, 0x61, 0x74, 0x65,
    0x00, 0x71, 0x75, 0x69, 0x74, 0x00, 0x54, 0x65, 0x73, 0x74, 0x5F, 0x67, 0x65, 0x74, 0x5F, 0x71,
    0x75, 0x69, 0x74, 0x00, 0x74, 0x72, 0x61, 0x63, 0x65, 0x00, 0x54, 0x65, 0x73, 0x74, 0x5F, 0x67,
    0x65, 0x74, 0x5F, 0x74, 0x72, 0x61, 0x63, 0x65, 0x00, 0x6C, 0x61, 0x75, 0x6E, 0x63, 0x68, 0x65,
    0x64, 0x00, 0x54, 0x65, 0x73, 0x74, 0x5F, 0x67, 0x65, 0x74, 0x5F, 0x6C, 0x61, 0x75, 0x6E, 0x63,
    0x68, 0x65, 0x64, 0x00, 0x70, 0x72, 0x69, 0x6E, 0x74, 0x00, 0x73, 0x00, 0x49, 0x4E, 0x46, 0x4F,
    0x3A, 0x20, 0x00, 0x45, 0x52, 0x52, 0x4F, 0x52, 0x3A, 0x20, 0x00, 0x96, 0x04, 0x00, 0x08, 0x00,
    0x08, 0x01, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x00, 0x4E, 0x1D, 0x96, 0x02, 0x00, 0x08, 0x00, 0x1C,
    0x96, 0x04, 0x00, 0x08, 0x02, 0x08, 0x01, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x03, 0x4E, 0x4F, 0x96,
    0x02, 0x00, 0x08, 0x00, 0x1C, 0x96, 0x07, 0x00, 0x08, 0x04, 0x07, 0x00, 0x00, 0x00, 0x00, 0x43,
    0x4F, 0x96, 0x02, 0x00, 0x08, 0x00, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x04, 0x4E, 0x96, 0x04, 0x00,
    0x08, 0x05, 0x08, 0x01, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x06, 0x4E, 0x4F, 0x96, 0x02, 0x00, 0x08,
    0x00, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x04, 0x4E, 0x96, 0x04, 0x00, 0x08, 0x07, 0x08, 0x01, 0x1C,
    0x96, 0x02, 0x00, 0x08, 0x08, 0x4E, 0x4F, 0x96, 0x02, 0x00, 0x08, 0x00, 0x1C, 0x96, 0x02, 0x00,
    0x08, 0x04, 0x4E, 0x96, 0x04, 0x00, 0x08, 0x09, 0x08, 0x01, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x0A,
    0x4E, 0x4F, 0x96, 0x02, 0x00, 0x08, 0x00, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x04, 0x4E, 0x96, 0x04,
    0x00, 0x08, 0x0B, 0x08, 0x01, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x0C, 0x4E, 0x4F, 0x96, 0x04, 0x00,
    0x08, 0x0D, 0x08, 0x01, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x0D, 0x4E, 0x1D, 0x96, 0x02, 0x00, 0x08,
    0x0D, 0x1C, 0x96, 0x07, 0x00, 0x08, 0x04, 0x07, 0x00, 0x00, 0x00, 0x00, 0x43, 0x4F, 0x96, 0x02,
    0x00, 0x08, 0x0D, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x04, 0x4E, 0x96, 0x04, 0x00, 0x08, 0x0E, 0x08,
    0x01, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x0F, 0x4E, 0x4F, 0x96, 0x02, 0x00, 0x08, 0x0D, 0x1C, 0x96,
    0x02, 0x00, 0x08, 0x04, 0x4E, 0x96, 0x04, 0x00, 0x08, 0x10, 0x08, 0x01, 0x1C, 0x96, 0x02, 0x00,
    0x08, 0x11, 0x4E, 0x4F, 0x96, 0x02, 0x00, 0x08, 0x12, 0x9B, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x1D, 0x96, 0x02, 0x00, 0x08, 0x12, 0x1C, 0x96, 0x07, 0x00, 0x08, 0x04, 0x07, 0x00, 0x00,
    0x00, 0x00, 0x43, 0x4F, 0x96, 0x02, 0x00, 0x08, 0x12, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x04, 0x4E,
    0x96, 0x04, 0x00, 0x08, 0x13, 0x08, 0x01, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x14, 0x4E, 0x4F, 0x96,
    0x02, 0x00, 0x08, 0x12, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x04, 0x4E, 0x96, 0x04, 0x00, 0x08, 0x15,
    0x08, 0x01, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x16, 0x4E, 0x4F, 0x96, 0x02, 0x00, 0x08, 0x12, 0x1C,
    0x96, 0x02, 0x00, 0x08, 0x04, 0x4E, 0x96, 0x04, 0x00, 0x08, 0x17, 0x08, 0x01, 0x1C, 0x96, 0x02,
    0x00, 0x08, 0x18, 0x4E, 0x4F, 0x96, 0x03, 0x00, 0x02, 0x08, 0x01, 0x1C, 0x96, 0x02, 0x00, 0x08,
    0x1A, 0x4E, 0x96, 0x09, 0x00, 0x08, 0x19, 0x07, 0x03, 0x00, 0x00, 0x00, 0x08, 0x12, 0x1C, 0x96,
    0x02, 0x00, 0x08, 0x04, 0x4E, 0x96, 0x02, 0x00, 0x08, 0x1B, 0x52, 0x17, 0x96, 0x04, 0x00, 0x08,
    0x1C, 0x08, 0x01, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x1C, 0x4E, 0x1D, 0x96, 0x02, 0x00, 0x08, 0x1C,
    0x1C, 0x96, 0x07, 0x00, 0x08, 0x04, 0x07, 0x00, 0x00, 0x00, 0x00, 0x43, 0x4F, 0x96, 0x02, 0x00,
    0x08, 0x1C, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x04, 0x4E, 0x96, 0x04, 0x00, 0x08, 0x1D, 0x08, 0x01,
    0x1C, 0x96, 0x02, 0x00, 0x08, 0x1E, 0x4E, 0x4F, 0x96, 0x02, 0x00, 0x08, 0x1C, 0x1C, 0x96, 0x02,
    0x00, 0x08, 0x04, 0x4E, 0x96, 0x04, 0x00, 0x08, 0x1F, 0x08, 0x01, 0x1C, 0x96, 0x02, 0x00, 0x08,
    0x20, 0x4E, 0x4F, 0x96, 0x02, 0x00, 0x08, 0x1C, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x04, 0x4E, 0x96,
    0x04, 0x00, 0x08, 0x21, 0x08, 0x01, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x22, 0x4E, 0x4F, 0x96, 0x02,
    0x00, 0x08, 0x1C, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x04, 0x4E, 0x96, 0x04, 0x00, 0x08, 0x23, 0x08,
    0x01, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x24, 0x4E, 0x4F, 0x96, 0x02, 0x00, 0x08, 0x1C, 0x1C, 0x96,
    0x02, 0x00, 0x08, 0x04, 0x4E, 0x96, 0x04, 0x00, 0x08, 0x25, 0x08, 0x01, 0x1C, 0x96, 0x02, 0x00,
    0x08, 0x26, 0x4E, 0x4F, 0x96, 0x02, 0x00, 0x08, 0x1C, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x04, 0x4E,
    0x96, 0x04, 0x00, 0x08, 0x27, 0x08, 0x01, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x28, 0x4E, 0x4F, 0x96,
    0x03, 0x00, 0x02, 0x08, 0x01, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x2A, 0x4E, 0x96, 0x09, 0x00, 0x08,
    0x29, 0x07, 0x03, 0x00, 0x00, 0x00, 0x08, 0x1C, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x04, 0x4E, 0x96,
    0x02, 0x00, 0x08, 0x1B, 0x52, 0x17, 0x96, 0x03, 0x00, 0x02, 0x08, 0x01, 0x1C, 0x96, 0x02, 0x00,
    0x08, 0x2C, 0x4E, 0x96, 0x09, 0x00, 0x08, 0x2B, 0x07, 0x03, 0x00, 0x00, 0x00, 0x08, 0x1C, 0x1C,
    0x96, 0x02, 0x00, 0x08, 0x04, 0x4E, 0x96, 0x02, 0x00, 0x08, 0x1B, 0x52, 0x17, 0x96, 0x03, 0x00,
    0x02, 0x08, 0x01, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x2E, 0x4E, 0x96, 0x09, 0x00, 0x08, 0x2D, 0x07,
    0x03, 0x00, 0x00, 0x00, 0x08, 0x1C, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x04, 0x4E, 0x96, 0x02, 0x00,
    0x08, 0x1B, 0x52, 0x17, 0x96, 0x03, 0x00, 0x02, 0x08, 0x01, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x30,
    0x4E, 0x96, 0x09, 0x00, 0x08, 0x2F, 0x07, 0x03, 0x00, 0x00, 0x00, 0x08, 0x1C, 0x1C, 0x96, 0x02,
    0x00, 0x08, 0x04, 0x4E, 0x96, 0x02, 0x00, 0x08, 0x1B, 0x52, 0x17, 0x96, 0x02, 0x00, 0x08, 0x31,
    0x9B, 0x07, 0x00, 0x00, 0x01, 0x00, 0x73, 0x00, 0x27, 0x00, 0x96, 0x02, 0x00, 0x08, 0x32, 0x1C,
    0x12, 0x9D, 0x02, 0x00, 0x1B, 0x00, 0x96, 0x04, 0x00, 0x08, 0x33, 0x08, 0x32, 0x1C, 0x47, 0x96,
    0x07, 0x00, 0x07, 0x01, 0x00, 0x00, 0x00, 0x08, 0x01, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x31, 0x52,
    0x17, 0x1D, 0x96, 0x02, 0x00, 0x08, 0x15, 0x9B, 0x07, 0x00, 0x00, 0x01, 0x00, 0x73, 0x00, 0x27,
    0x00, 0x96, 0x02, 0x00, 0x08, 0x32, 0x1C, 0x12, 0x9D, 0x02, 0x00, 0x1B, 0x00, 0x96, 0x04, 0x00,
    0x08, 0x34, 0x08, 0x32, 0x1C, 0x47, 0x96, 0x07, 0x00, 0x07, 0x01, 0x00, 0x00, 0x00, 0x08, 0x01,
    0x1C, 0x96, 0x02, 0x00, 0x08, 0x31, 0x52, 0x17, 0x1D, 0x00
};

static const unsigned char null_text[] = "";
static const char* TEXT_PLAIN = "text/plain";

#define DEFAULT_MIMETYPE TEXT_PLAIN;

inline void send_headers(const char* mime_type, int length)
{
    printf("%s %d %s\r\n", "HTTP/1.0", 200, "OK");
    printf("Server: %s\r\n", "nullserv");

    if (mime_type)
        printf("Content-Type: %s\r\n", mime_type);

    if (length >= 0)
        printf("Content-Length: %d\r\n", length);

    printf("Connection: close\r\n");
    printf("\r\n");
}

inline const char* get_mime_type(const char* ext)
{
    if (!ext)                                                  return DEFAULT_MIMETYPE;
    if (!strcasecmp(ext, ".gif"))                              return "image/gif";
    if (!strcasecmp(ext, ".png"))                              return "image/png";
    if (!strcasecmp(ext, ".jpg") || !strcasecmp(ext, ".jpeg")) return "image/jpeg";
    if (!strcasecmp(ext, ".swf"))                              return "application/x-shockwave-flash";
    if (!strcasecmp(ext, ".js" ) || !strcasecmp(ext, ".ajax")) return "text/javascript";
    if (!strcasecmp(ext, ".htm") || !strcasecmp(ext, ".html")) return "text/html";
    if (!strcasecmp(ext, ".css"))                              return "text/css";
    if (!strcasecmp(ext, ".php"))                              return TEXT_PLAIN;
    if (!strcasecmp(ext, ".asp") || !strcasecmp(ext, ".aspx")) return TEXT_PLAIN;
    if (!strcasecmp(ext, ".cgi") || !strcasecmp(ext, ".pl"  )) return TEXT_PLAIN;
    if (!strcasecmp(ext, ".txt"))                              return TEXT_PLAIN;
    return DEFAULT_MIMETYPE;
}

int main(int argc, char** argv)
{
    char buf[4096];
    char* method;
    char* path;
    char* protocol;
    char* querystring;
    char* file;
    const char* ext;
    const char* mime_type;
    size_t size;
    const unsigned char* content;

//    setlogmask (LOG_UPTO (LOG_NOTICE));
//    openlog ("nullserv", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_USER);

    if (!fgets(buf, sizeof(buf), stdin))
    {
//        syslog(LOG_NOTICE, "Failed to read from stdin (nothing to read?)");
//        closelog();
        return -1;
    }

    // Get the individual bits of the request
    method = strtok(buf, " ");
    path = strtok(NULL, " ");
    protocol = strtok(NULL, "\r");

    // Get the request file extenstion and mime type.
    file = strtok(path, "?");
    ext = strrchr(file, '.');
    mime_type = get_mime_type(ext);

    // Parse querystring if there is one.
    querystring = strstr(path, "?");
    if (querystring) ++querystring;
/*
    syslog(LOG_NOTICE, "argc     : %d", argc);
    syslog(LOG_NOTICE, "buf      : %s", buf);
    syslog(LOG_NOTICE, "\tProtocol : %s %s", protocol, method);
    syslog(LOG_NOTICE, "\tPath     : %s", path);
    syslog(LOG_NOTICE, "\tFile     : %s", file);
    syslog(LOG_NOTICE, "\tExt      : %s", ext);
    syslog(LOG_NOTICE, "\tQuery    : %s", querystring);
    syslog(LOG_NOTICE, "\tMimeType : %s", mime_type);
    closelog();
*/
    // Server up some null content.
    if (!strcasecmp(mime_type, TEXT_PLAIN)  ||
        !strcasecmp(mime_type, "text/html") ||
        !strcasecmp(mime_type, "text/css")  ||
        !strcasecmp(mime_type, "text/javascript"))
    {
        content = null_text;
        size = sizeof(null_text);
    }
    else if (!strcasecmp(mime_type, "image/gif"))
    {
        content = null_gif;
        size = sizeof(null_gif);
    }
    else if (!strcasecmp(mime_type, "image/png"))
    {
        content = null_png;
        size = sizeof(null_png);
    }
    else if (!strcasecmp(mime_type, "image/jpeg"))
    {
        content = null_jpg;
        size = sizeof(null_jpg);
    }
    else if (!strcasecmp(mime_type, "application/x-shockwave-flash"))
    {
        content = null_swf;
        size = sizeof(null_swf);
    }
    else
    {
        content = null_text;
        size = sizeof(null_text);
    }

    send_headers(mime_type, (size - 1));
    fwrite(content, 1, (size - 1), stdout);

    return 0;
}
