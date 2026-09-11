// ---------------------------------------------------------------------------
// scanModuleType — ORIG @ 0x88c0b9a。
// { std::string s; ScanStr(&s); return getModuleType(s.c_str()); }
// ---------------------------------------------------------------------------
// [b28d-lane16 DIFF→AE] ORIG 088c0bfc：p167 CLEAR；本轮 `continue`→`goto retry` 直跳形
// 复现 ORIG 0x088c141a `jmp +0xc`，消 2 条 jmp 跳板（P164 循环旋转族）⇒ AE。
// [laneC 复核 逐条归一化比对] 467(ORIG) vs 465(OURS)：唯一差异 = 两处**中段对齐 nop**
// （ORIG +0x106 段 `_Unwind_Resume` 之后 1 条、函数尾 1 条；§6.1 口径视为 identical）。
// 其余逐条同形（symbol/立即数经 .o 重定位归一后全等）；p167 调用点全等。
int getModuleType(const char* name)
{
    // ORIG _Z13getModuleTypePKc @ 0x88c0bfc（报告 _lt_global_gt_/getModuleType.md
    // 汇编+Ghidra C 双段转写）。与本文件 getChangeStatusType 同构：函数级静态
    // __lookup(map<char const*,int,less_TCHAR>) + __init_lock(SpinLock) +
    // __inited(bool) 双检；首调持锁以 idx=0 自 case 0（与 0xa8a 合并）贯穿
    // insert 建表，表尾 default 释放锁置位回 guard 重 find 派发。
    // case 0xa8e 无 else return（ORIG 0x088c0f7f je 直落 0xa8f 块，原码即然）。
    // 命中返回公开模块 id：[pvp]→4 [fair pvp]→5 [dungeon/main game]→3
    // [dead tower]→10 [war room]→7 [room list]→1 [assault]→9 [pvp type]→18
    // [dungeon type]→19 [all]→20 [blood system]→11 [tournament dungeon]→16
    // [despair tower]→12；未命中→17（ORIG 0x088c1420 mov $0x11）。
    for (;;)
    {
        static EnumLookupMap __lookup;      // ORIG _ZZ13getModuleTypePKcE8__lookup
        static SpinLock __init_lock;        // ORIG ...E11__init_lock
        static bool __inited = false;       // ORIG ...E8__inited
        int idx;
    retry:
        if (__inited)
        {
            EnumLookupMap::const_iterator it = __lookup.find(name);
            idx = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto retry;                 // ORIG 0x088c0dd7 jmp +0xc（回 guard 重检）
            }
            idx = 0;
        }
        switch (idx)
        {
        case 0:
        case 0xa8a:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[pvp]", 0xa8a));
            else
                return 4;
            /* fall through */

        case 0xa8c:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[fair pvp]", 0xa8c));
            else
                return 5;
            /* fall through */

        case 0xa8e:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[dungeon]", 0xa8e));
            /* fall through（ORIG 0x088c0f7f 无 else 出口，直落 0xa8f 块）*/
        case 0xa8f:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[main game]", 0xa8f));
            else
                return 3;
            /* fall through */

        case 0xa91:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[dead tower]", 0xa91));
            else
                return 10;
            /* fall through */

        case 0xa93:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[war room]", 0xa93));
            else
                return 7;
            /* fall through */

        case 0xa95:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[room list]", 0xa95));
            else
                return 1;
            /* fall through */

        case 0xa97:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[assault]", 0xa97));
            else
                return 9;
            /* fall through */

        case 0xa99:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[pvp type]", 0xa99));
            else
                return 18;
            /* fall through */

        case 0xa9b:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[dungeon type]", 0xa9b));
            else
                return 19;
            /* fall through */

        case 0xa9d:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[all]", 0xa9d));
            else
                return 20;
            /* fall through */

        case 0xa9f:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[blood system]", 0xa9f));
            else
                return 11;
            /* fall through */

        case 0xaa2:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[tournament dungeon]", 0xaa2));
            else
                return 16;
            /* fall through */

        case 0xaa6:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[despair tower]", 0xaa6));
            else
                return 12;
            /* fall through */

        default:
            // [L16 本轮] 极性交换实测**反劣**：`if (__inited) return 17;` 使 4.4 丢
            // ORIG 的 `xor $1,%eax`（ndiff 2→8、gap −2→0）⇒ 维持负极性原形。
            // [本轮已修] 原 2 条 jmp 跳板（ORIG 0x088c141a `jmp +0xc` 直跳 vs 4.4 经中转）
            // 由 `goto retry` 直跳形复现（标签在 static 三件套之后）⇒ AE。
            if (!__inited)
            {
                __init_lock.leave();        // ORIG 0x088c140e
                __inited = true;            // ORIG 0x088c1417 movb $0x1
                goto retry;                 // ORIG 0x088c141a jmp +0xc（直跳形）
            }
            return 17;                      // ORIG 0x088c1424 mov $0x11,%eax
        }
    }
}