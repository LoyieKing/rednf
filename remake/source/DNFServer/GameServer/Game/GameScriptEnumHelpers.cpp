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

// ---------------------------------------------------------------------------
// getStateToEnumValue — ORIG _Z19getStateToEnumValueRSs @ 0x89bbc05（10 字节）：
// 函数体仅 mov $-1,%eax; ret —— 所有输入恒返回 -1。
// ---------------------------------------------------------------------------
int getStateToEnumValue(std::string& /*s*/)
{
    return -1;
}
// ---------------------------------------------------------------------------
// getActiveStatusToEnumValue — ORIG @ 0x88c29a2，20 个 strcmp 分支 → 0..19，默认 -1。
// ---------------------------------------------------------------------------
int getActiveStatusToEnumValue(std::string& s)
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
int getTargetToEnumValue(std::string& s)
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
int getAttackConditionToEnumValue(std::string& s)
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
int getCategoryToEnumValue(std::string& s)
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
int getElementToEnumValue(std::string& s)
{
    if (s == "fire")  return 0;
    if (s == "water") return 1;
    if (s == "dark")  return 2;
    if (s == "light") return 3;
    if (s == "none")  return 4;
    if (s == "all")   return 4;
    return -1;
}

// ---------------------------------------------------------------------------
// getGradeToEnumValue — ORIG @ 0x89bcd5b，4 个分支，默认 4。
// ---------------------------------------------------------------------------
int getGradeToEnumValue(std::string& s)
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
int getObjectTypeToEnumValue(std::string& s)
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
// ---------------------------------------------------------------------------
int getChangeStatusType(std::string& s)
{
    for (;;)
    {
        static EnumLookupMap __lookup;      // ORIG _ZZ19getChangeStatusTypeRSsE8__lookup
        static SpinLock __init_lock;        // ORIG ...E11__init_lock
        static bool __inited = false;       // ORIG ...E8__inited
        int idx;
        if (!__inited)
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                continue;                   // ORIG 08892cfc jmp +0xc（回 guard 重检）
            }
            idx = 0;
        }
        else
        {
            EnumLookupMap::const_iterator it = __lookup.find(s.c_str());
            idx = (it != __lookup.end()) ? it->second : -1;
        }
        switch (idx)
        {
        // 建表模式（首调 !__inited，自 case 0 进入）逐 case 贯穿 insert 全表；
        // 派发模式（__inited）命中即 return（ORIG 各块 mov $N,%eax; jmp ret）。
        case 0:
        case 0x294:
            if (__inited) return 0;
            __lookup.insert(EnumLookupMap::value_type("physical attack", 0x294));
            /* fall through */
        case 0x296:
            if (__inited) return 1;
            __lookup.insert(EnumLookupMap::value_type("magical attack", 0x296));
            /* fall through */
        case 0x298:
            if (__inited) return 3;
            __lookup.insert(EnumLookupMap::value_type("physical defense", 0x298));
            /* fall through */
        case 0x29a:
            if (__inited) return 2;
            __lookup.insert(EnumLookupMap::value_type("magical defense", 0x29a));
            /* fall through */
        case 0x29c:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("equipment physical attack", 0x29c));
            /* fall through（ORIG 088931bb 无 else 出口，直落 0x29d）*/
        case 0x29d:
            if (__inited) return 4;
            __lookup.insert(EnumLookupMap::value_type("physical equipment attack", 0x29d));
            /* fall through */
        case 0x29f:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("equipment physical defense", 0x29f));
            /* fall through */
        case 0x2a0:
            if (__inited) return 5;
            __lookup.insert(EnumLookupMap::value_type("physical equipment defense", 0x2a0));
            /* fall through */
        case 0x2a2:
            if (__inited) return 8;
            __lookup.insert(EnumLookupMap::value_type("jump power", 0x2a2));
            /* fall through */
        case 0x2a4:
            if (__inited) return 10;
            __lookup.insert(EnumLookupMap::value_type("attack speed", 0x2a4));
            /* fall through */
        case 0x2a6:
            if (__inited) return 11;
            __lookup.insert(EnumLookupMap::value_type("move speed", 0x2a6));
            /* fall through */
        case 0x2a8:
            if (__inited) return 12;
            __lookup.insert(EnumLookupMap::value_type("cast speed", 0x2a8));
            /* fall through */
        case 0x2aa:
            if (__inited) return 13;
            __lookup.insert(EnumLookupMap::value_type("hp max", 0x2aa));
            /* fall through */
        case 0x2ac:
            if (__inited) return 14;
            __lookup.insert(EnumLookupMap::value_type("hp regen rate", 0x2ac));
            /* fall through */
        case 0x2ae:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("magical critical hit rate", 0x2ae));
            /* fall through */
        case 0x2af:
            if (__inited) return 16;
            __lookup.insert(EnumLookupMap::value_type("magical critical hit", 0x2af));
            /* fall through */
        case 0x2b1:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("physical critical hit rate", 0x2b1));
            /* fall through */
        case 0x2b2:
            if (__inited) return 15;
            __lookup.insert(EnumLookupMap::value_type("physical critical hit", 0x2b2));
            /* fall through */
        case 0x2b4:
            if (!__inited)
                __lookup.insert(
                    EnumLookupMap::value_type("physical back attack critical rate", 0x2b4));
            /* fall through */
        case 0x2b5:
            if (__inited) return 17;
            __lookup.insert(EnumLookupMap::value_type("physical back attack critical", 0x2b5));
            /* fall through */
        case 0x2b7:
            if (!__inited)
                __lookup.insert(
                    EnumLookupMap::value_type("magical back attack critical rate", 0x2b7));
            /* fall through */
        case 0x2b8:
            // ORIG 0889384a：与 0x2b7 同串（原表笔误照录），insert 仍各一条。
            if (__inited) return 18;
            __lookup.insert(EnumLookupMap::value_type("magical back attack critical rate", 0x2b8));
            /* fall through */
        case 0x2ba:
            if (__inited) return 20;
            __lookup.insert(EnumLookupMap::value_type("fire element", 0x2ba));
            /* fall through */
        case 0x2bc:
            if (__inited) return 21;
            __lookup.insert(EnumLookupMap::value_type("water element", 0x2bc));
            /* fall through */
        case 0x2be:
            if (__inited) return 22;
            __lookup.insert(EnumLookupMap::value_type("dark element", 0x2be));
            /* fall through */
        case 0x2c0:
            if (__inited) return 23;
            __lookup.insert(EnumLookupMap::value_type("light element", 0x2c0));
            /* fall through */
        case 0x2c2:
            if (__inited) return 33;
            __lookup.insert(EnumLookupMap::value_type("stuck", 0x2c2));
            /* fall through */
        case 0x2c4:
            if (__inited) return 27;
            __lookup.insert(EnumLookupMap::value_type("stuck resistance", 0x2c4));
            /* fall through */
        case 0x2c7:
            if (__inited) return 48;
            __lookup.insert(EnumLookupMap::value_type("mp max", 0x2c7));
            /* fall through */
        case 0x2ca:
            if (__inited) return 29;
            __lookup.insert(EnumLookupMap::value_type("mp regen rate", 0x2ca));
            /* fall through */
        case 0x2cc:
            if (__inited) return 30;
            __lookup.insert(EnumLookupMap::value_type("extra speed rate", 0x2cc));
            /* fall through */
        case 0x2ce:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("equipment magical attack", 0x2ce));
            /* fall through */
        case 0x2cf:
            if (__inited) return 31;
            __lookup.insert(EnumLookupMap::value_type("magical equipment attack", 0x2cf));
            /* fall through */
        case 0x2d1:
            if (!__inited)
                __lookup.insert(EnumLookupMap::value_type("equipment magical defense", 0x2d1));
            /* fall through */
        case 0x2d2:
            if (__inited) return 32;
            __lookup.insert(EnumLookupMap::value_type("magical equipment defense", 0x2d2));
            /* fall through */
        case 0x2d4:
            if (__inited) return 20;
            __lookup.insert(EnumLookupMap::value_type("fire resistance", 0x2d4));
            /* fall through */
        case 0x2d6:
            if (__inited) return 21;
            __lookup.insert(EnumLookupMap::value_type("water resistance", 0x2d6));
            /* fall through */
        case 0x2d8:
            if (__inited) return 22;
            __lookup.insert(EnumLookupMap::value_type("dark resistance", 0x2d8));
            /* fall through */
        case 0x2da:
            if (__inited) return 23;
            __lookup.insert(EnumLookupMap::value_type("light resistance", 0x2da));
            /* fall through */
        case 0x2dc:
            if (__inited) return 24;
            __lookup.insert(EnumLookupMap::value_type("all elemental resistance", 0x2dc));
            /* fall through */
        case 0x2de:
            if (__inited) return 26;
            __lookup.insert(EnumLookupMap::value_type("all activestatus resistance", 0x2de));
            /* fall through */
        case 0x2e0:
            if (__inited) return 41;
            __lookup.insert(EnumLookupMap::value_type("rigidity", 0x2e0));
            /* fall through */
        case 0x2e2:
            if (__inited) return 42;
            __lookup.insert(EnumLookupMap::value_type("fire attack", 0x2e2));
            /* fall through */
        case 0x2e4:
            if (__inited) return 43;
            __lookup.insert(EnumLookupMap::value_type("water attack", 0x2e4));
            /* fall through */
        case 0x2e6:
            if (__inited) return 44;
            __lookup.insert(EnumLookupMap::value_type("dark attack", 0x2e6));
            /* fall through */
        case 0x2e8:
            if (__inited) return 45;
            __lookup.insert(EnumLookupMap::value_type("light attack", 0x2e8));
            /* fall through */
        case 0x2eb:
            if (__inited) return 53;
            __lookup.insert(EnumLookupMap::value_type("separate attack", 0x2eb));
            /* fall through */
        case 0x2f4:
            if (__inited) return 55;
            __lookup.insert(EnumLookupMap::value_type("antievil", 0x2f4));
            /* fall through */
        case 0x2f7:
            if (__inited) return 49;
            __lookup.insert(EnumLookupMap::value_type("skill cool time", 0x2f7));
            /* fall through */
        case 0x2f9:
            if (__inited) return 56;
            __lookup.insert(EnumLookupMap::value_type("physical absolute damage", 0x2f9));
            /* fall through */
        case 0x2fb:
            if (__inited) return 57;
            __lookup.insert(EnumLookupMap::value_type("no breath time change", 0x2fb));
            /* fall through */
        case 0x2fd:
            if (__inited) return 6;
            __lookup.insert(EnumLookupMap::value_type("physical attack bonus", 0x2fd));
            /* fall through */
        case 0x2ff:
            if (__inited) return 7;
            __lookup.insert(EnumLookupMap::value_type("magical attack bonus", 0x2ff));
        }
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            continue;                       // ORIG 088944e5 jmp +0xc 重派发
        }
        return 0x4d;                        // ORIG 088944eb mov $0x4d,%eax
    }
}

// ---------------------------------------------------------------------------
// getNpcRoleByString — ORIG @ 0x89fecee（按 std::string 值传参）。
// ORIG 结构（/tmp/getNpcRoleByString.md 汇编+Ghidra C 双段实证）：函数级静态
// __lookup = map<char const*, int, less_TCHAR>（__cxa_guard 惰性构造）+
// __init_lock(SpinLock) + __inited 双检；未建表时持锁以 idx=0 进入 switch，
// 自 case 0（与 case 0x706 合并）起逐 case **贯穿** insert(value_type(键,码))
// 共 56 条，表尾释放锁、置 __inited 并跳回 guard 重新 find 派发；建表后命中
// case 写 result 并 break 返回角色 0..55，未命中保持入口初值 -1。
// ---------------------------------------------------------------------------
int getNpcRoleByString(std::string name)
{
    int result = -1;                        // ORIG 089fecfa movl $0xffffffff,-0x20(%ebp)
    for (;;)
    {
        static EnumLookupMap __lookup;      // ORIG _ZZ18getNpcRoleByStringSsE8__lookup
        static SpinLock __init_lock;        // ORIG ...E11__init_lock
        static bool __inited = false;       // ORIG ...E8__inited
        int idx;
        if (!__inited)
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                continue;                   // ORIG 089fee04 重检回 guard
            }
            idx = 0;
        }
        else
        {
            EnumLookupMap::const_iterator it = __lookup.find(name.c_str());
            idx = (it != __lookup.end()) ? it->second : -1;
        }
        switch (idx)
        {
        // 建表模式（首调 !__inited，自 case 0 进入）逐 case 贯穿 insert 全表；
        // 派发模式命中写 result 后 break（ORIG 各块 movl $N,-0x20(%ebp); jmp done）。
        case 0:
        case 0x706:
            if (__inited) { result = 0; break; }
            __lookup.insert(EnumLookupMap::value_type("[item shop]", 0x706));
            /* fall through */
        case 0x70b:
            if (__inited) { result = 1; break; }
            __lookup.insert(EnumLookupMap::value_type("[recover stamina]", 0x70b));
            /* fall through */
        case 0x710:
            if (__inited) { result = 2; break; }
            __lookup.insert(EnumLookupMap::value_type("[sera shop]", 0x710));
            /* fall through */
        case 0x715:
            if (__inited) { result = 3; break; }
            __lookup.insert(EnumLookupMap::value_type("[guild]", 0x715));
            /* fall through */
        case 0x71a:
            if (__inited) { result = 4; break; }
            __lookup.insert(EnumLookupMap::value_type("[upgrade item]", 0x71a));
            /* fall through */
        case 0x720:
            if (__inited) { result = 5; break; }
            __lookup.insert(EnumLookupMap::value_type("[upgrade item event]", 0x720));
            /* fall through */
        case 0x726:
            if (__inited) { result = 6; break; }
            __lookup.insert(EnumLookupMap::value_type("[disjoint item]", 0x726));
            /* fall through */
        case 0x72b:
            if (__inited) { result = 7; break; }
            __lookup.insert(EnumLookupMap::value_type("[product item]", 0x72b));
            /* fall through */
        case 0x730:
            if (__inited) { result = 8; break; }
            __lookup.insert(EnumLookupMap::value_type("[power war]", 0x730));
            /* fall through */
        case 0x735:
            if (__inited) { result = 9; break; }
            __lookup.insert(EnumLookupMap::value_type("[move town]", 0x735));
            /* fall through */
        case 0x73a:
            if (__inited) { result = 10; break; }
            __lookup.insert(EnumLookupMap::value_type("[item storage]", 0x73a));
            /* fall through */
        case 0x73f:
            if (__inited) { result = 11; break; }
            __lookup.insert(EnumLookupMap::value_type("[post box]", 0x73f));
            /* fall through */
        case 0x744:
            if (__inited) { result = 12; break; }
            __lookup.insert(EnumLookupMap::value_type("[coke vending machine]", 0x744));
            /* fall through */
        case 0x749:
            if (__inited) { result = 13; break; }
            __lookup.insert(EnumLookupMap::value_type("[mouse register]", 0x749));
            /* fall through */
        case 0x74e:
            if (__inited) { result = 14; break; }
            __lookup.insert(EnumLookupMap::value_type("[security card make]", 0x74e));
            /* fall through */
        case 0x753:
            if (__inited) { result = 15; break; }
            __lookup.insert(EnumLookupMap::value_type("[security card remake]", 0x753));
            /* fall through */
        case 0x758:
            if (__inited) { result = 16; break; }
            __lookup.insert(EnumLookupMap::value_type("[security card delete]", 0x758));
            /* fall through */
        case 0x75e:
            if (__inited) { result = 17; break; }
            __lookup.insert(EnumLookupMap::value_type("[automat]", 0x75e));
            /* fall through */
        case 0x772:
            if (__inited) { result = 18; break; }
            __lookup.insert(EnumLookupMap::value_type("[just a cake]", 0x772));
            /* fall through */
        case 0x778:
            if (__inited) { result = 19; break; }
            __lookup.insert(EnumLookupMap::value_type("[convert avatar]", 0x778));
            /* fall through */
        case 0x77d:
            if (__inited) { result = 20; break; }
            __lookup.insert(EnumLookupMap::value_type("[compound emblem]", 0x77d));
            /* fall through */
        case 0x782:
            if (__inited) { result = 21; break; }
            __lookup.insert(EnumLookupMap::value_type("[amplify item]", 0x782));
            /* fall through */
        case 0x787:
            if (__inited) { result = 22; break; }
            __lookup.insert(EnumLookupMap::value_type("[coin vending machine]", 0x787));
            /* fall through */
        case 0x795:
            if (__inited) { result = 43; break; }
            __lookup.insert(EnumLookupMap::value_type("[event halloween witch]", 0x795));
            /* fall through */
        case 0x79a:
            if (__inited) { result = 44; break; }
            __lookup.insert(EnumLookupMap::value_type("[event halloween caldron]", 0x79a));
            /* fall through */
        case 0x7af:
            if (__inited) { result = 23; break; }
            __lookup.insert(EnumLookupMap::value_type("[event message board]", 0x7af));
            /* fall through */
        case 0x7b5:
            if (__inited) { result = 24; break; }
            __lookup.insert(EnumLookupMap::value_type("[donate power war point]", 0x7b5));
            /* fall through */
        case 0x7ba:
            if (__inited) { result = 25; break; }
            __lookup.insert(EnumLookupMap::value_type("[create guild agit]", 0x7ba));
            /* fall through */
        case 0x7bf:
            if (__inited) { result = 26; break; }
            __lookup.insert(EnumLookupMap::value_type("[delete guild agit]", 0x7bf));
            /* fall through */
        case 0x7c4:
            if (__inited) { result = 27; break; }
            __lookup.insert(EnumLookupMap::value_type("[enter guild agit]", 0x7c4));
            /* fall through */
        case 0x7c9:
            if (__inited) { result = 28; break; }
            __lookup.insert(EnumLookupMap::value_type("[exit guild agit]", 0x7c9));
            /* fall through */
        case 0x7ce:
            if (__inited) { result = 29; break; }
            __lookup.insert(EnumLookupMap::value_type("[upgrade guild agit]", 0x7ce));
            /* fall through */
        case 0x7d3:
            if (__inited) { result = 30; break; }
            __lookup.insert(EnumLookupMap::value_type("[housing guild agit]", 0x7d3));
            /* fall through */
        case 0x7d8:
            if (__inited) { result = 31; break; }
            __lookup.insert(EnumLookupMap::value_type("[guild storage]", 0x7d8));
            /* fall through */
        case 0x7df:
            if (__inited) { result = 32; break; }
            __lookup.insert(EnumLookupMap::value_type("[powerwar statue]", 0x7df));
            /* fall through */
        case 0x7e5:
            if (__inited) { result = 33; break; }
            __lookup.insert(EnumLookupMap::value_type("[disjoint repair]", 0x7e5));
            /* fall through */
        case 0x7ea:
            if (__inited) { result = 34; break; }
            __lookup.insert(EnumLookupMap::value_type("[disjoint upgrade]", 0x7ea));
            /* fall through */
        case 0x7ef:
            if (__inited) { result = 49; break; }
            __lookup.insert(EnumLookupMap::value_type("[play quest movie]", 0x7ef));
            /* fall through */
        case 0x7f6:
            if (__inited) { result = 51; break; }
            __lookup.insert(EnumLookupMap::value_type("[play bead movie]", 0x7f6));
            /* fall through */
        case 0x7fd:
            if (__inited) { result = 35; break; }
            __lookup.insert(EnumLookupMap::value_type("[job change]", 0x7fd));
            /* fall through */
        case 0x803:
            if (__inited) { result = 36; break; }
            __lookup.insert(EnumLookupMap::value_type("[cera shop inventory]", 0x803));
            /* fall through */
        case 0x80b:
            if (__inited) { result = 38; break; }
            __lookup.insert(EnumLookupMap::value_type("[sudden shop]", 0x80b));
            /* fall through */
        case 0x81b:
            if (__inited) { result = 40; break; }
            __lookup.insert(EnumLookupMap::value_type("[SANSENBAKO]", 0x81b));
            /* fall through */
        case 0x830:
            if (__inited) { result = 37; break; }
            __lookup.insert(EnumLookupMap::value_type("[random option operate]", 0x830));
            /* fall through */
        case 0x854:
            if (__inited) { result = 41; break; }
            __lookup.insert(EnumLookupMap::value_type("[contest reception]", 0x854));
            /* fall through */
        case 0x85b:
            if (__inited) { result = 42; break; }
            __lookup.insert(EnumLookupMap::value_type("[genuine damage upgrade]", 0x85b));
            /* fall through */
        case 0x869:
            if (__inited) { result = 45; break; }
            __lookup.insert(EnumLookupMap::value_type("[image communication]", 0x869));
            /* fall through */
        case 0x870:
            if (__inited) { result = 46; break; }
            __lookup.insert(EnumLookupMap::value_type("[tutorial dungeon lvup]", 0x870));
            /* fall through */
        case 0x877:
            if (__inited) { result = 47; break; }
            __lookup.insert(EnumLookupMap::value_type("[crane minigame]", 0x877));
            /* fall through */
        case 0x87e:
            if (__inited) { result = 48; break; }
            __lookup.insert(EnumLookupMap::value_type("[time sand]", 0x87e));
            /* fall through */
        case 0x885:
            if (__inited) { result = 39; break; }
            __lookup.insert(EnumLookupMap::value_type("[repair item]", 0x885));
            /* fall through */
        case 0x88c:
            if (__inited) { result = 50; break; }
            __lookup.insert(EnumLookupMap::value_type("[blue marble]", 0x88c));
            /* fall through */
        case 0x89f:
            if (__inited) { result = 55; break; }
            __lookup.insert(EnumLookupMap::value_type("[one a day shop]", 0x89f));
            /* fall through */
        case 0x8a6:
            if (__inited) { result = 52; break; }
            __lookup.insert(EnumLookupMap::value_type("[secret shop]", 0x8a6));
            /* fall through */
        case 0x8e1:
            if (__inited) { result = 53; break; }
            __lookup.insert(EnumLookupMap::value_type("[convert avatar jpn]", 0x8e1));
            /* fall through */
        case 0x8e8:
            if (__inited) { result = 54; break; }
            __lookup.insert(EnumLookupMap::value_type("[compound emblem jpn]", 0x8e8));
        }
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            continue;                       // ORIG 08a008f1 回 guard 重新 find 派发
        }
        return result;                      // ORIG 08a008f7 mov -0x20(%ebp),%eax
    }
}

// ---------------------------------------------------------------------------
// getNpcMoodFromString — ORIG @ 0x89faf46。
// strncmp(name, X, 10)：good→2、normal→1、bad→0、其余→3。
// ---------------------------------------------------------------------------
int getNpcMoodFromString(const char* name)
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
        if (!__inited)
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                continue;                   // ORIG 0x088c0dd7 jmp +0xc（回 guard 重检）
            }
            idx = 0;
        }
        else
        {
            EnumLookupMap::const_iterator it = __lookup.find(name);
            idx = (it != __lookup.end()) ? it->second : -1;
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
            if (!__inited)
            {
                __init_lock.leave();        // ORIG 0x088c140e
                __inited = true;            // ORIG 0x088c1417 movb $0x1
                continue;                   // ORIG 0x088c141a jmp +0xc
            }
            return 17;                      // ORIG 0x088c1424 mov $0x11,%eax
        }
    }
}

int scanModuleType()
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
static int gCodePage = 0;   // ORIG _ZL9gCodePage 0x09501e90

int setCodePage(int nNewCodePage)
{
    const int old = gCodePage;
    gCodePage = nNewCodePage;
    return old;
}

// getCodePage —— ORIG @ 0x8adea10：mov 0x9501e90,%eax 直返全局 codepage。
int getCodePage()
{
    return gCodePage;
}

// setCodePageByLocal — ORIG @ 0x808ec64：setCodePage(950 /*0x3b6*/)。
void setCodePageByLocal()
{
    setCodePage(950);
}
