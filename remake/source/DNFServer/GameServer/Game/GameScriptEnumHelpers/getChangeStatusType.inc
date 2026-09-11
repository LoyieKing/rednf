// ---------------------------------------------------------------------------
// getChangeStatusType — ORIG @ 0x8892b0d。
// ORIG 结构（/tmp/getChangeStatusType.md 汇编+Ghidra C 双段实证）：函数级静态
// __lookup = map<char const*, int, less_TCHAR>（__cxa_guard 惰性构造）+
// __init_lock(SpinLock) + __inited 双检；未建表时持锁以 idx=0 进入 switch，
// 自 case 0（与 case 0x294 合并）起逐 case **贯穿** insert(value_type(键,码))
// 共 53 条，落到表尾释放锁、置 __inited 并跳回 guard 重新 find 派发；建表后
// find 命中按 case 直接 return change-status 枚举，未命中/未知码 return 0x4d。
// 「merge」case（0x29c/0x29f/0x2ae/0x2b1/0x2b4/0x2b7/0x2ce/0x2d1）无独立返回值，
// 命中后落入下一 case 的 return（ORIG 汇编 je 直落下一块，逐条转写）。
// 【batch-9 收敛】case 体改 `if (!__inited){insert} else return N;` xor-bool 岛形
// （ORIG 每块 movzbl+xor$1+test，0x08892e53 起）+ 外层翻为 if(__inited){find}
// else{enter 双检}：ours 1294 → 1383 == ORIG 1383（残余仅 .o relocation 显示，
// relink 后按 AE 口径核销）。
// ---------------------------------------------------------------------------
// [b28d-lane16 DIFF→AE] ORIG 08892b0d：p167 CLEAR；本轮把 guard 回跳的 `continue`
// 改 `goto retry`（标签置于 static 三件套之后、`if (__inited)` 之前）⇒ 复现 ORIG
// 0x088944e5 `jmp +0xc` 直跳，消 4.4 的 2 条 jmp 中转跳板（P164 循环旋转族）。
// [laneC 复核] 逐条归一化比对 1383=1381：残差仅 2 条**中段对齐 nop**（§6.1 豁免）；
// p167 调用点全等（含 56 条 insert/惰性注册链）⇒ 维持 AE 判词。
int getChangeStatusType(std::string& s)
{
    for (;;)
    {
        static EnumLookupMap __lookup;      // ORIG _ZZ19getChangeStatusTypeRSsE8__lookup
        static SpinLock __init_lock;        // ORIG ...E11__init_lock
        static bool __inited = false;       // ORIG ...E8__inited
                int idx;
    retry:
        if (__inited)
        {
            EnumLookupMap::const_iterator it = __lookup.find(s.c_str());
            idx = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto retry;
            }
            idx = 0;
        }
        switch (idx)
        {
        // 建表模式（首调 !__inited，自 case 0 进入）逐 case 贯穿 insert 全表；
        // 派发模式（__inited）命中即 return（ORIG 各块 mov $N,%eax; jmp ret）。
        case 0:
        case 0x294:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("physical attack", 0x294));
            else
                return 0;
            /* fall through */
        case 0x296:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("magical attack", 0x296));
            else
                return 1;
            /* fall through */
        case 0x298:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("physical defense", 0x298));
            else
                return 3;
            /* fall through */
        case 0x29a:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("magical defense", 0x29a));
            else
                return 2;
            /* fall through */
        case 0x29c:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("equipment physical attack", 0x29c));
            /* fall through（ORIG 088931bb 无 else 出口，直落 0x29d）*/
        case 0x29d:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("physical equipment attack", 0x29d));
            else
                return 4;
            /* fall through */
        case 0x29f:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("equipment physical defense", 0x29f));
            /* fall through */
        case 0x2a0:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("physical equipment defense", 0x2a0));
            else
                return 5;
            /* fall through */
        case 0x2a2:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("jump power", 0x2a2));
            else
                return 8;
            /* fall through */
        case 0x2a4:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("attack speed", 0x2a4));
            else
                return 10;
            /* fall through */
        case 0x2a6:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("move speed", 0x2a6));
            else
                return 11;
            /* fall through */
        case 0x2a8:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("cast speed", 0x2a8));
            else
                return 12;
            /* fall through */
        case 0x2aa:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("hp max", 0x2aa));
            else
                return 13;
            /* fall through */
        case 0x2ac:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("hp regen rate", 0x2ac));
            else
                return 14;
            /* fall through */
        case 0x2ae:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("magical critical hit rate", 0x2ae));
            /* fall through */
        case 0x2af:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("magical critical hit", 0x2af));
            else
                return 16;
            /* fall through */
        case 0x2b1:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("physical critical hit rate", 0x2b1));
            /* fall through */
        case 0x2b2:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("physical critical hit", 0x2b2));
            else
                return 15;
            /* fall through */
        case 0x2b4:
            if (!__inited)
                __lookup.insert(
                    EnumLookupMap::value_type("physical back attack critical rate", 0x2b4));
            /* fall through */
        case 0x2b5:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("physical back attack critical", 0x2b5));
            else
                return 17;
            /* fall through */
        case 0x2b7:
            if (!__inited)
                __lookup.insert(
                    EnumLookupMap::value_type("magical back attack critical rate", 0x2b7));
            /* fall through */
        case 0x2b8:
            // ORIG 0889384a：与 0x2b7 同串（原表笔误照录），insert 仍各一条。
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("magical back attack critical rate", 0x2b8));
            else
                return 18;
            /* fall through */
        case 0x2ba:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("fire element", 0x2ba));
            else
                return 20;
            /* fall through */
        case 0x2bc:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("water element", 0x2bc));
            else
                return 21;
            /* fall through */
        case 0x2be:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("dark element", 0x2be));
            else
                return 22;
            /* fall through */
        case 0x2c0:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("light element", 0x2c0));
            else
                return 23;
            /* fall through */
        case 0x2c2:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("stuck", 0x2c2));
            else
                return 33;
            /* fall through */
        case 0x2c4:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("stuck resistance", 0x2c4));
            else
                return 27;
            /* fall through */
        case 0x2c7:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("mp max", 0x2c7));
            else
                return 48;
            /* fall through */
        case 0x2ca:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("mp regen rate", 0x2ca));
            else
                return 29;
            /* fall through */
        case 0x2cc:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("extra speed rate", 0x2cc));
            else
                return 30;
            /* fall through */
        case 0x2ce:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("equipment magical attack", 0x2ce));
            /* fall through */
        case 0x2cf:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("magical equipment attack", 0x2cf));
            else
                return 31;
            /* fall through */
        case 0x2d1:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("equipment magical defense", 0x2d1));
            /* fall through */
        case 0x2d2:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("magical equipment defense", 0x2d2));
            else
                return 32;
            /* fall through */
        case 0x2d4:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("fire resistance", 0x2d4));
            else
                return 20;
            /* fall through */
        case 0x2d6:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("water resistance", 0x2d6));
            else
                return 21;
            /* fall through */
        case 0x2d8:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("dark resistance", 0x2d8));
            else
                return 22;
            /* fall through */
        case 0x2da:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("light resistance", 0x2da));
            else
                return 23;
            /* fall through */
        case 0x2dc:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("all elemental resistance", 0x2dc));
            else
                return 24;
            /* fall through */
        case 0x2de:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("all activestatus resistance", 0x2de));
            else
                return 26;
            /* fall through */
        case 0x2e0:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("rigidity", 0x2e0));
            else
                return 41;
            /* fall through */
        case 0x2e2:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("fire attack", 0x2e2));
            else
                return 42;
            /* fall through */
        case 0x2e4:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("water attack", 0x2e4));
            else
                return 43;
            /* fall through */
        case 0x2e6:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("dark attack", 0x2e6));
            else
                return 44;
            /* fall through */
        case 0x2e8:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("light attack", 0x2e8));
            else
                return 45;
            /* fall through */
        case 0x2eb:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("separate attack", 0x2eb));
            else
                return 53;
            /* fall through */
        case 0x2f4:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("antievil", 0x2f4));
            else
                return 55;
            /* fall through */
        case 0x2f7:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("skill cool time", 0x2f7));
            else
                return 49;
            /* fall through */
        case 0x2f9:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("physical absolute damage", 0x2f9));
            else
                return 56;
            /* fall through */
        case 0x2fb:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("no breath time change", 0x2fb));
            else
                return 57;
            /* fall through */
        case 0x2fd:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("physical attack bonus", 0x2fd));
            else
                return 6;
            /* fall through */
        case 0x2ff:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("magical attack bonus", 0x2ff));
            else
                return 7;
        }
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto retry;                       // ORIG 088944e5 jmp +0xc 重派发
        }
        return 0x4d;                        // ORIG 088944eb mov $0x4d,%eax
    }
}