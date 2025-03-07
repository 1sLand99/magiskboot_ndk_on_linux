/*
 * Copyright (C) 2015 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

extern "C" int sys_faccessat(int, const char*, int);

int faccessat(int dirfd, const char* pathname, int mode, int flags) {
  // "The mode specifies the accessibility check(s) to be performed,
  // and is either the value F_OK, or a mask consisting of the
  // bitwise OR of one or more of R_OK, W_OK, and X_OK."
  if ((mode != F_OK) && ((mode & ~(R_OK | W_OK | X_OK)) != 0) &&
      ((mode & (R_OK | W_OK | X_OK)) == 0)) {
    errno = EINVAL;
    return -1;
  }

  if (flags != 0) {
    // We deliberately don't support AT_SYMLINK_NOFOLLOW, a glibc
    // only feature which is error prone and dangerous.
    // More details at http://permalink.gmane.org/gmane.linux.lib.musl.general/6952
    //
    // AT_EACCESS isn't supported either. Android doesn't have setuid
    // programs, and never runs code with euid!=uid.
    //
    // We could use faccessat2(2) from Linux 5.8, but since we don't want the
    // first feature and don't need the second, we just reject such requests.
    errno = EINVAL;
    return -1;
  }

  return sys_faccessat(dirfd, pathname, mode);
}
