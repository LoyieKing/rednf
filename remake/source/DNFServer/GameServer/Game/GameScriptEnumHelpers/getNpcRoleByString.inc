// ---------------------------------------------------------------------------
// getNpcRoleByString — ORIG @ 0x89fecee（按 std::string 值传参）。
// 【batch-9 收敛】case 体改 `if (!__inited){insert} else {result=N;break;}`
// xor-bool 岛形（ORIG 每块 movzbl+xor$1+test）+ 外层 if(__inited){find}
// else{enter 双检}：ours 1364 → 1475（ORIG 1476；唯一实质残差 = lock 区
// 2 条 4.1 块对齐 nop，负例表空 else nop;jmp 岛族；relocation 显示差待
// relink 核销）。
// ORIG 结构（/tmp/getNpcRoleByString.md 汇编+Ghidra C 双段实证）：函数级静态
// __lookup = map<char const*, int, less_TCHAR>（__cxa_guard 惰性构造）+
// __init_lock(SpinLock) + __inited 双检；未建表时持锁以 idx=0 进入 switch，
// 自 case 0（与 case 0x706 合并）起逐 case **贯穿** insert(value_type(键,码))
// 共 56 条，表尾释放锁、置 __inited 并跳回 guard 重新 find 派发；建表后命中
// case 写 result 并 break 返回角色 0..55，未命中保持入口初值 -1。
// ---------------------------------------------------------------------------
// [b28d-lane16 DIFF→AE] ORIG 089fecee：`goto retry` 直跳形消 4.4 jmp 中转跳板后 AE（同 getModuleType 修法；旧「ours 1364 → 1475，残差 = lock 区
// [laneC 复核] 逐条归一化比对 1476=1474：残差仅 2 条**中段对齐 nop**（§6.1 豁免）；
// p167 调用点全等 ⇒ 维持 AE 判词。
int getNpcRoleByString(std::string name)
{
    int result = -1;                        // ORIG 089fecfa movl $0xffffffff,-0x20(%ebp)
    for (;;)
    {
    retry:
        // [R17a19 2026-09-13 · 回跳目标对齐] ORIG 的两处 `goto retry` 落点 =
        //   __lookup 的 **__cxa_guard 检查首条**（0x089fed01 `mov $0x94fe7d0,%eax`；
        //   表尾回跳 0x08a008f1 同靶）。原写 `retry:` 落在 static 声明之后 ⇒ 4.4 把
        //   goto 目标定在 `__inited` 检查（我方 0x85e971a `movzbl __inited`），
        //   跳过了 guard 检查。标签前移（goto 跳过 static 声明合法）即落回 guard
        //   检查首条（P333 状态机 retry 锚点形）。
        static EnumLookupMap __lookup;      // ORIG _ZZ18getNpcRoleByStringSsE8__lookup
        static SpinLock __init_lock;        // ORIG ...E11__init_lock
        static bool __inited = false;       // ORIG ...E8__inited
                int idx;
        if (__inited)
        {
            EnumLookupMap::const_iterator it = __lookup.find(name.c_str());
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
        // 派发模式命中写 result 后 break（ORIG 各块 movl $N,-0x20(%ebp); jmp done）。
        case 0:
        case 0x706:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[item shop]", 0x706));
            else { result = 0; break; }
            /* fall through */
        case 0x70b:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[recover stamina]", 0x70b));
            else { result = 1; break; }
            /* fall through */
        case 0x710:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[sera shop]", 0x710));
            else { result = 2; break; }
            /* fall through */
        case 0x715:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[guild]", 0x715));
            else { result = 3; break; }
            /* fall through */
        case 0x71a:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[upgrade item]", 0x71a));
            else { result = 4; break; }
            /* fall through */
        case 0x720:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[upgrade item event]", 0x720));
            else { result = 5; break; }
            /* fall through */
        case 0x726:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[disjoint item]", 0x726));
            else { result = 6; break; }
            /* fall through */
        case 0x72b:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[product item]", 0x72b));
            else { result = 7; break; }
            /* fall through */
        case 0x730:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[power war]", 0x730));
            else { result = 8; break; }
            /* fall through */
        case 0x735:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[move town]", 0x735));
            else { result = 9; break; }
            /* fall through */
        case 0x73a:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[item storage]", 0x73a));
            else { result = 10; break; }
            /* fall through */
        case 0x73f:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[post box]", 0x73f));
            else { result = 11; break; }
            /* fall through */
        case 0x744:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[coke vending machine]", 0x744));
            else { result = 12; break; }
            /* fall through */
        case 0x749:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[mouse register]", 0x749));
            else { result = 13; break; }
            /* fall through */
        case 0x74e:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[security card make]", 0x74e));
            else { result = 14; break; }
            /* fall through */
        case 0x753:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[security card remake]", 0x753));
            else { result = 15; break; }
            /* fall through */
        case 0x758:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[security card delete]", 0x758));
            else { result = 16; break; }
            /* fall through */
        case 0x75e:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[automat]", 0x75e));
            else { result = 17; break; }
            /* fall through */
        case 0x772:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[just a cake]", 0x772));
            else { result = 18; break; }
            /* fall through */
        case 0x778:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[convert avatar]", 0x778));
            else { result = 19; break; }
            /* fall through */
        case 0x77d:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[compound emblem]", 0x77d));
            else { result = 20; break; }
            /* fall through */
        case 0x782:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[amplify item]", 0x782));
            else { result = 21; break; }
            /* fall through */
        case 0x787:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[coin vending machine]", 0x787));
            else { result = 22; break; }
            /* fall through */
        case 0x795:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[event halloween witch]", 0x795));
            else { result = 43; break; }
            /* fall through */
        case 0x79a:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[event halloween caldron]", 0x79a));
            else { result = 44; break; }
            /* fall through */
        case 0x7af:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[event message board]", 0x7af));
            else { result = 23; break; }
            /* fall through */
        case 0x7b5:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[donate power war point]", 0x7b5));
            else { result = 24; break; }
            /* fall through */
        case 0x7ba:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[create guild agit]", 0x7ba));
            else { result = 25; break; }
            /* fall through */
        case 0x7bf:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[delete guild agit]", 0x7bf));
            else { result = 26; break; }
            /* fall through */
        case 0x7c4:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[enter guild agit]", 0x7c4));
            else { result = 27; break; }
            /* fall through */
        case 0x7c9:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[exit guild agit]", 0x7c9));
            else { result = 28; break; }
            /* fall through */
        case 0x7ce:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[upgrade guild agit]", 0x7ce));
            else { result = 29; break; }
            /* fall through */
        case 0x7d3:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[housing guild agit]", 0x7d3));
            else { result = 30; break; }
            /* fall through */
        case 0x7d8:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[guild storage]", 0x7d8));
            else { result = 31; break; }
            /* fall through */
        case 0x7df:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[powerwar statue]", 0x7df));
            else { result = 32; break; }
            /* fall through */
        case 0x7e5:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[disjoint repair]", 0x7e5));
            else { result = 33; break; }
            /* fall through */
        case 0x7ea:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[disjoint upgrade]", 0x7ea));
            else { result = 34; break; }
            /* fall through */
        case 0x7ef:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[play quest movie]", 0x7ef));
            else { result = 49; break; }
            /* fall through */
        case 0x7f6:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[play bead movie]", 0x7f6));
            else { result = 51; break; }
            /* fall through */
        case 0x7fd:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[job change]", 0x7fd));
            else { result = 35; break; }
            /* fall through */
        case 0x803:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[cera shop inventory]", 0x803));
            else { result = 36; break; }
            /* fall through */
        case 0x80b:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[sudden shop]", 0x80b));
            else { result = 38; break; }
            /* fall through */
        case 0x81b:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[SANSENBAKO]", 0x81b));
            else { result = 40; break; }
            /* fall through */
        case 0x830:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[random option operate]", 0x830));
            else { result = 37; break; }
            /* fall through */
        case 0x854:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[contest reception]", 0x854));
            else { result = 41; break; }
            /* fall through */
        case 0x85b:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[genuine damage upgrade]", 0x85b));
            else { result = 42; break; }
            /* fall through */
        case 0x869:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[image communication]", 0x869));
            else { result = 45; break; }
            /* fall through */
        case 0x870:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[tutorial dungeon lvup]", 0x870));
            else { result = 46; break; }
            /* fall through */
        case 0x877:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[crane minigame]", 0x877));
            else { result = 47; break; }
            /* fall through */
        case 0x87e:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[time sand]", 0x87e));
            else { result = 48; break; }
            /* fall through */
        case 0x885:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[repair item]", 0x885));
            else { result = 39; break; }
            /* fall through */
        case 0x88c:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[blue marble]", 0x88c));
            else { result = 50; break; }
            /* fall through */
        case 0x89f:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[one a day shop]", 0x89f));
            else { result = 55; break; }
            /* fall through */
        case 0x8a6:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[secret shop]", 0x8a6));
            else { result = 52; break; }
            /* fall through */
        case 0x8e1:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[convert avatar jpn]", 0x8e1));
            else { result = 53; break; }
            /* fall through */
        case 0x8e8:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("[compound emblem jpn]", 0x8e8));
            else { result = 54; break; }
        }
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto retry;                       // ORIG 08a008f1 回 guard 重新 find 派发
        }
        return result;                      // ORIG 08a008f7 mov -0x20(%ebp),%eax
    }
}