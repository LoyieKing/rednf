// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GameScriptEnumHelpers/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// GameScriptEnumHelpers.cpp — 字符串→枚举映射自由函数。
// 逐函数依据 ORIG df_game_r objdump 反汇编还原（地址见各函数注释）；
// getChangeStatusType / getNpcRoleByString 按 ORIG 结构还原为「函数级静态
// map<char const*,int,less_TCHAR> + SpinLock 双检 + 贯穿式 insert 建表链 +
// switch(码) 派发」原形（键/码/返回值逐条转写自反汇编）。
// 注意：getCharacterJobToEnumValue(0x89bbc0f) 由 CDataManager.cpp 提供，不在此重复。

#include "GameScriptEnumHelpers.h"
#include "DNFLexWrapper.h"   // ScanStr(std::string*)
#include "CEnvironment.h"    // less_TCHAR / SpinLock（ORIG __lookup 静态表依赖）

#include <map>
#include <cstring>

// getChangeStatusType / getNpcRoleByString 的函数级静态查找表类型：
// ORIG 实证 std::map<char const*, int, less_TCHAR>（键按串内容比较）。
typedef std::map<const char*, int, less_TCHAR> EnumLookupMap;

// [b28 P210] 调用者定义移到 callee 之前：4.4 对「TU 内已见定义的 callee」按其实
// 帧精确继承 outgoing（塌缩 sub $0x4）；ORIG 0x0808ec67 帧 0x18 为 callee 未见时
// 保守形。前置定义后恢复保守大帧。
// setCodePageByLocal — ORIG @ 0x808ec64：setCodePage(950 /*0x3b6*/)。
#include "GameScriptEnumHelpers/setCodePageByLocal.inc"
{
    setCodePage(950);
}

// ---------------------------------------------------------------------------
// getStateToEnumValue — ORIG _Z19getStateToEnumValueRSs @ 0x89bbc05（10 字节）：
// 函数体仅 mov $-1,%eax; ret —— 所有输入恒返回 -1。
// ---------------------------------------------------------------------------
#include "GameScriptEnumHelpers/getStateToEnumValue.inc"
{
    return -1;
}
// ---------------------------------------------------------------------------
// getActiveStatusToEnumValue — ORIG @ 0x88c29a2，20 个 strcmp 分支 → 0..19，默认 -1。
// ---------------------------------------------------------------------------
#include "GameScriptEnumHelpers/getActiveStatusToEnumValue.inc"
{
    // ORIG 0x88c29a2：20 个 if 链 strcmp 分支 → 0..19，默认 -1（非循环查表）。
    if (s == "slow")        return 0;
    if (s == "freeze")      return 1;
    if (s == "poison")      return 2;
    if (s == "stun")        return 3;
    if (s == "curse")       return 4;
    if (s == "blind")       return 5;
    if (s == "lightning")   return 6;
    if (s == "stone")       return 7;
    if (s == "sleep")       return 8;
    if (s == "burn")        return 9;
    if (s == "weapon break") return 10;
    if (s == "bleeding")    return 11;
    if (s == "haste")       return 12;
    if (s == "bless")       return 13;
    if (s == "element")     return 14;
    if (s == "confuse")     return 15;
    if (s == "hold")        return 16;
    if (s == "armor break") return 17;
    if (s == "any")         return 18;
    if (s == "all")         return 19;
    return -1;
}

// ---------------------------------------------------------------------------
// getTargetToEnumValue — ORIG @ 0x88c2c34，10 个分支 → 0..9，
// 全部未命中时 ORIG 尾部 mov $0xa,%eax：默认返回 10。
// ---------------------------------------------------------------------------
// ORIG @ 0x88c2c34：10 个 if 链（s == 字面量），非查表循环。
#include "GameScriptEnumHelpers/getTargetToEnumValue.inc"
{
    if (s == "myself")
        return 0;
    if (s == "enemy")
        return 1;
    if (s == "party")
        return 2;
    if (s == "random party")
        return 3;
    if (s == "all enemy")
        return 4;
    if (s == "enemy hp max")
        return 5;
    if (s == "enemy hp min")
        return 6;
    if (s == "enemy near")
        return 7;
    if (s == "enemy far")
        return 8;
    if (s == "random enemy")
        return 9;
    return 10; // ORIG 默认分支
}

// ---------------------------------------------------------------------------
// getAttackConditionToEnumValue — ORIG @ 0x89bcb6d，4 个分支，默认 4。
// ---------------------------------------------------------------------------
#include "GameScriptEnumHelpers/getAttackConditionToEnumValue.inc"
{
    if (s == "critical")     return 0;
    if (s == "counter")      return 1;
    if (s == "back attack")  return 2;
    if (s == "stuck")        return 3;
    return 4;
}

// ---------------------------------------------------------------------------
// getCategoryToEnumValue — ORIG @ 0x89bcbf2，11 个分支（无 5），默认 0x2c(44)。
// ---------------------------------------------------------------------------
#include "GameScriptEnumHelpers/getCategoryToEnumValue.inc"
{
    if (s == "human")   return 0;
    if (s == "beast")   return 1;
    if (s == "plant")   return 2;
    if (s == "insect")  return 3;
    if (s == "machine") return 4;
    if (s == "hybrid")  return 6;
    if (s == "undead")  return 7;
    if (s == "devil")   return 8;
    if (s == "spirit")  return 9;
    if (s == "dragon")  return 10;
    if (s == "angel")   return 11;
    return 44; // ORIG 默认分支 mov $0x2c,%eax
}

// ---------------------------------------------------------------------------
// getElementToEnumValue — ORIG @ 0x89bca42，默认 -1。
// ---------------------------------------------------------------------------
#include "GameScriptEnumHelpers/getElementToEnumValue.inc"
{
    // ORIG 0x089bca42：比较序为 water→fire→dark→light→none→all（Ghidra C 实证）
    if (s == "water") return 1;
    if (s == "fire")  return 0;
    if (s == "dark")  return 2;
    if (s == "light") return 3;
    if (s == "none")  return 4;
    if (s == "all")   return 4;
    return -1;
}

// ---------------------------------------------------------------------------
// getGradeToEnumValue — ORIG @ 0x89bcd5b，4 个分支，默认 4。
// ---------------------------------------------------------------------------
#include "GameScriptEnumHelpers/getGradeToEnumValue.inc"
{
    if (s == "common")   return 0;
    if (s == "champion") return 1;
    if (s == "boss")     return 2;
    if (s == "named")    return 3;
    return 4;
}

// ---------------------------------------------------------------------------
// getObjectTypeToEnumValue — ORIG @ 0x89bbd99，默认 -1。
// （33=passive 对象、17=active 对象、529=monster、273=character，ORIG 立即数）
// ---------------------------------------------------------------------------
#include "GameScriptEnumHelpers/getObjectTypeToEnumValue.inc"
{
    if (s == "passive")   return 33;
    if (s == "active")    return 17;
    if (s == "monster")   return 529;
    if (s == "character") return 273;
    return -1;
}

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
#include "GameScriptEnumHelpers/getChangeStatusType.inc"
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
#include "GameScriptEnumHelpers/getNpcRoleByString.inc"
{
    int result = -1;                        // ORIG 089fecfa movl $0xffffffff,-0x20(%ebp)
    for (;;)
    {
        static EnumLookupMap __lookup;      // ORIG _ZZ18getNpcRoleByStringSsE8__lookup
        static SpinLock __init_lock;        // ORIG ...E11__init_lock
        static bool __inited = false;       // ORIG ...E8__inited
                int idx;
    retry:
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

// ---------------------------------------------------------------------------
// getNpcMoodFromString — ORIG @ 0x89faf46。
// strncmp(name, X, 10)：good→2、normal→1、bad→0、其余→3。
// ---------------------------------------------------------------------------
#include "GameScriptEnumHelpers/getNpcMoodFromString.inc"
{
    if (std::strncmp("good", name, 10) == 0)   return 2;
    if (std::strncmp("normal", name, 10) == 0) return 1;
    if (std::strncmp("bad", name, 10) == 0)    return 0;
    return 3;
}

// ---------------------------------------------------------------------------
// scanModuleType — ORIG @ 0x88c0b9a。
// { std::string s; ScanStr(&s); return getModuleType(s.c_str()); }
// ---------------------------------------------------------------------------
// [b28d-lane16 DIFF→AE] ORIG 088c0bfc：p167 CLEAR；本轮 `continue`→`goto retry` 直跳形
// 复现 ORIG 0x088c141a `jmp +0xc`，消 2 条 jmp 跳板（P164 循环旋转族）⇒ AE。
// [laneC 复核 逐条归一化比对] 467(ORIG) vs 465(OURS)：唯一差异 = 两处**中段对齐 nop**
// （ORIG +0x106 段 `_Unwind_Resume` 之后 1 条、函数尾 1 条；§6.1 口径视为 identical）。
// 其余逐条同形（symbol/立即数经 .o 重定位归一后全等）；p167 调用点全等。
#include "GameScriptEnumHelpers/getModuleType.inc"
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

#include "GameScriptEnumHelpers/scanModuleType.inc"
{
    std::string token;
    ScanStr(&token);
    return getModuleType(token.c_str());
}

// ---------------------------------------------------------------------------
// setCodePage — ORIG @ 0x8adea1a：全局 codepage 变量置换，返回旧值。
// （Library/Include/Core/UnicodeConvert.cpp 有同名语义实现，但 game 目标未编入，
//   故按 ORIG 在此提供唯一定义。）
// ---------------------------------------------------------------------------
// [b28d-Main R3 迁移·检查点13] gCodePage/getCodePage/setCodePage 已迁入 StringConvert.cpp
// （ORIG 0x09501e90/0x08adea10/0x08adea1a 归属 TU）⇒ 消本 TU 副本，修复码页语义分叉。

// setCodePage 定义已迁入 StringConvert.cpp（声明见 GameScriptEnumHelpers.h）。
