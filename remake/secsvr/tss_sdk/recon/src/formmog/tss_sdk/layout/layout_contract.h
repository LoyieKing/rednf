// Compile-time layout contract helpers for the canonical ORIG layouts.
//
// The reconstruction is built with -std=gnu++98 (build_server.py), where
// `static_assert` is not a keyword.  TSS_SDK_STATIC_ASSERT keeps the source
// spelling of a static assertion and falls back to the standard C++98
// negative-array-bound idiom, which fails the build the same way.
#ifndef TSS_SDK_LAYOUT_CONTRACT_H_
#define TSS_SDK_LAYOUT_CONTRACT_H_

#include <stddef.h>
// The offset contracts below intentionally apply offsetof() to classes that
// are not POD in C++98 (user-declared constructors, virtual destructors).
// GCC 4.1 - the ORIG compiler - had no standard-layout notion, and every
// expected offset was read from the ORIG DWARF, so the C++98
// "conditionally-supported" diagnostic is suppressed here.
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif


#if defined(__cplusplus) && __cplusplus >= 201103L
#define TSS_SDK_STATIC_ASSERT(cond, tag) static_assert((cond), #tag)
#else
#define TSS_SDK_STATIC_ASSERT(cond, tag) \
    typedef char tss_sdk_static_assert_##tag[(cond) ? 1 : -1]
#endif

#define TSS_SDK_OFFSETOF(type, member) __builtin_offsetof(type, member)

// Member offsets have to be asserted from inside a member function of the
// class: offsetof() of a private member is rejected outside the class, and the
// class is only complete once its body is closed.  A private static member
// function that is never called adds no data member and no virtual slot, so
// the layout stays ORIG-identical.
#define TSS_SDK_OFFSET_CONTRACT(type, member, expected, tag) \
    TSS_SDK_STATIC_ASSERT(TSS_SDK_OFFSETOF(type, member) == (expected), tag)

#endif  // TSS_SDK_LAYOUT_CONTRACT_H_
