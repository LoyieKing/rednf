// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + docs/class_func_reports/StringResourceDelayedItem/（8 函数，T 强符号）。
// 布局证据（ctor 0x08ad52a2 汇编）：+0x00 char const* 缓存指针（初始 0）、
// +0x04 CharString（sizeof 4，指针型 buffer_）、+0x08 StringResource const*、
// +0x0c int id → sizeof 0x10。
#ifndef DNF_GAME_STRINGRESOURCEDELAYEDITEM_H_
#define DNF_GAME_STRINGRESOURCEDELAYEDITEM_H_

#include "StringTypes.h"

class StringResource;  // 前置声明（参数/指针仅用引用与地址）

class StringResourceDelayedItem
{
public:
    StringResourceDelayedItem(StringResource const&, int);  // T 0x08ad52a2
    ~StringResourceDelayedItem();                           // T 0x08ad52d4

    bool get(CharString const**) const;               // T 0x08ad5302（asm test %al）
    int getId() const;                                // T 0x08ad52f6（返回 +0x0c）
    StringResource const* getStringResource() const;  // T 0x08ad52ea（返回 +0x08）【推断】指针返回
    void makeCacheStr();                              // T 0x08ad538a；返回类型【推断】void
    operator CharString() const;                      // T 0x08ad532a
    operator char const*() const;                     // T 0x08ad5368

private:
    char const* m_pCache;          // +0x00（惰性缓存，初始 0）
    CharString m_cacheStr;         // +0x04
    StringResource const* m_pRes;  // +0x08
    int m_id;                      // +0x0c
};
typedef char assert_strresdelayed_size[(sizeof(StringResourceDelayedItem) == 0x10) ? 1 : -1];

#endif  // DNF_GAME_STRINGRESOURCEDELAYEDITEM_H_
