// Copyright 2020 The Fuchsia Authors
//
// Use of this source code is governed by a MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT

#ifndef LIB_SPECIAL_SECTIONS_INCLUDE_LIB_SPECIAL_SECTIONS_SPECIAL_SECTIONS_H_
#define LIB_SPECIAL_SECTIONS_INCLUDE_LIB_SPECIAL_SECTIONS_SPECIAL_SECTIONS_H_

// This is used as a C++ attribute on a variable definition to specify it goes
// into a special section.  The section name is given as a string.  The type is
// the element type of the section, for determining alignment.  Variables in
// special sections get precise alignment so that the whole section is packed
// as an array.  Without the alignas attribute, the compiler is allowed to
// overalign any variable, which would break the array-like layout.  Without
// the used attribute, the compiler will remove it (and maybe warn) if it's not
// referenced in the file.  Without the retain attribute, the linker might
// remove it if it's not referenced by anything else in the link.  Using them
// all ensures that just the declaration being compiled in will guarantee the
// element appears in the special section at runtime.
#define SPECIAL_SECTION(name, type)                                                 \
  alignas(type) [[gnu::used, gnu::retain, gnu::section(SPECIAL_SECTION_NAME(name)), \
                  SPECIAL_SECTION_NO_ASAN]]

#ifdef _WIN32
#define SPECIAL_SECTION_NAME(name) "." name "$M"
#else
#define SPECIAL_SECTION_NAME(name) name
#endif

// AddressSanitizer instrumentation normally places red zones around global
// variables.  This must be suppressed in special sections so as not to break
// the array-like layout.
#ifdef __clang__
#define SPECIAL_SECTION_NO_ASAN clang::no_sanitize("address")
#else
#define SPECIAL_SECTION_NO_ASAN
#endif

#endif  // LIB_SPECIAL_SECTIONS_INCLUDE_LIB_SPECIAL_SECTIONS_SPECIAL_SECTIONS_H_
