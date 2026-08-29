// GameScriptEnumHelpers.cpp — 字符串→枚举映射自由函数。
// 逐函数依据 ORIG df_game_r objdump 反汇编还原（地址见各函数注释）；
// getChangeStatusType / getNpcRoleByString 在 ORIG 中为「静态 map<const char*,int>
// 首调惰性建表 + switch(idx) 返回角色值」，此处等价实现为直接 string→返回值
// 映射表（默认值与 ORIG 一致），标注【推断】处为等价改写。
// 注意：getCharacterJobToEnumValue(0x89bbc0f) 由 CDataManager.cpp 提供，不在此重复。

#include "GameScriptEnumHelpers.h"
#include "DNFLexWrapper.h"   // ScanStr(std::string*)

#include <map>
#include <cstring>

namespace {

typedef std::map<std::string, int> StrIntMap;

} // namespace

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
// ORIG 结构：静态 map<"字符串", 表内 idx>（首调建表）→ find 得 idx →
// switch(idx) 返回 change-status 枚举；find 失败/未知 idx 默认 0x4d(77)。
// 下表为「字符串 → 返回值」直接映射（别名拼写保留，与 ORIG 行为一致）。
// ---------------------------------------------------------------------------
int getChangeStatusType(std::string& s)
{
    static const struct { const char* name; int value; } table[] = {
        {"physical attack", 0},
        {"magical attack", 1},
        {"physical defense", 3},
        {"magical defense", 2},
        {"equipment physical attack", 4},   // 别名（ORIG 同 case 链）
        {"physical equipment attack", 4},
        {"equipment physical defense", 5},  // 别名
        {"physical equipment defense", 5},
        {"physical attack bonus", 6},
        {"magical attack bonus", 7},
        {"jump power", 8},
        {"attack speed", 10},
        {"move speed", 11},
        {"cast speed", 12},
        {"hp max", 13},
        {"hp regen rate", 14},
        {"magical critical hit rate", 16},  // 别名
        {"magical critical hit", 16},
        {"physical critical hit rate", 15}, // 别名
        {"physical critical hit", 15},
        {"physical back attack critical rate", 17}, // 别名
        {"physical back attack critical", 17},
        {"magical back attack critical rate", 18},  // ORIG 两处同串
        {"fire element", 20},
        {"water element", 21},
        {"dark element", 22},
        {"light element", 23},
        {"stuck", 33},
        {"stuck resistance", 27},
        {"mp max", 48},
        {"mp regen rate", 29},
        {"extra speed rate", 30},
        {"equipment magical attack", 31},   // 别名
        {"magical equipment attack", 31},
        {"equipment magical defense", 32},  // 别名
        {"magical equipment defense", 32},
        {"fire resistance", 20},
        {"water resistance", 21},
        {"dark resistance", 22},
        {"light resistance", 23},
        {"all elemental resistance", 24},
        {"all activestatus resistance", 26},
        {"rigidity", 41},
        {"fire attack", 42},
        {"water attack", 43},
        {"dark attack", 44},
        {"light attack", 45},
        {"separate attack", 53},
        {"antievil", 55},
        {"skill cool time", 49},
        {"physical absolute damage", 56},
        {"no breath time change", 57},
    };
    static StrIntMap tbl;
    if (tbl.empty())
    {
        const int n = static_cast<int>(sizeof(table) / sizeof(table[0]));
        for (int i = 0; i < n; ++i)
            tbl.insert(StrIntMap::value_type(table[i].name, table[i].value));
    }
    StrIntMap::const_iterator it = tbl.find(s);
    if (it != tbl.end())
        return it->second;
    return 77; // ORIG 默认分支 mov $0x4d,%eax
}

// ---------------------------------------------------------------------------
// getNpcRoleByString — ORIG @ 0x89fecee（按 std::string 值传参）。
// ORIG 结构：静态 map<const char*, 表内 idx> + SpinLock 首调建表 → find 得
// idx → switch(idx) 返回 NPC 角色 0..55；未命中保持入口初值 -1。
// 【推断】此处等价改写为「字符串 → 角色」直接映射。
// ---------------------------------------------------------------------------
int getNpcRoleByString(std::string name)
{
    static const struct { const char* name; int role; } table[] = {
        {"[item shop]", 0},
        {"[recover stamina]", 1},
        {"[sera shop]", 2},
        {"[guild]", 3},
        {"[upgrade item]", 4},
        {"[upgrade item event]", 5},
        {"[disjoint item]", 6},
        {"[product item]", 7},
        {"[power war]", 8},
        {"[move town]", 9},
        {"[item storage]", 10},
        {"[post box]", 11},
        {"[coke vending machine]", 12},
        {"[mouse register]", 13},
        {"[security card make]", 14},
        {"[security card remake]", 15},
        {"[security card delete]", 16},
        {"[automat]", 17},
        {"[just a cake]", 18},
        {"[convert avatar]", 19},
        {"[compound emblem]", 20},
        {"[amplify item]", 21},
        {"[coin vending machine]", 22},
        {"[event message board]", 23},
        {"[donate power war point]", 24},
        {"[create guild agit]", 25},
        {"[delete guild agit]", 26},
        {"[enter guild agit]", 27},
        {"[exit guild agit]", 28},
        {"[upgrade guild agit]", 29},
        {"[housing guild agit]", 30},
        {"[guild storage]", 31},
        {"[powerwar statue]", 32},
        {"[disjoint repair]", 33},
        {"[disjoint upgrade]", 34},
        {"[job change]", 35},
        {"[cera shop inventory]", 36},
        {"[random option operate]", 37},
        {"[sudden shop]", 38},
        {"[repair item]", 39},
        {"[SANSENBAKO]", 40},
        {"[contest reception]", 41},
        {"[genuine damage upgrade]", 42},
        {"[event halloween witch]", 43},
        {"[event halloween caldron]", 44},
        {"[image communication]", 45},
        {"[tutorial dungeon lvup]", 46},
        {"[crane minigame]", 47},
        {"[time sand]", 48},
        {"[play quest movie]", 49},
        {"[blue marble]", 50},
        {"[play bead movie]", 51},
        {"[secret shop]", 52},
        {"[convert avatar jpn]", 53},
        {"[compound emblem jpn]", 54},
        {"[one a day shop]", 55},
    };
    static StrIntMap tbl;
    if (tbl.empty())
    {
        const int n = static_cast<int>(sizeof(table) / sizeof(table[0]));
        for (int i = 0; i < n; ++i)
            tbl.insert(StrIntMap::value_type(table[i].name, table[i].role));
    }
    StrIntMap::const_iterator it = tbl.find(name);
    if (it != tbl.end())
        return it->second;
    return -1;
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
// getModuleType — ORIG _Z13getModuleTypePKc @ 0x88c0bfc (size 0x834)。
// ORIG：惰性注册表 map<字符串字面量, 模块枚举> + SpinLock；switch 把枚举换算为
// 公开模块 id。等价实现（按内容匹配，标【推断】——ORIG 按字面量指针匹配，
// 对脚本 token 场景行为一致）：
//   [pvp]→4 [fair pvp]→5 [dungeon]→3 [main game]→3 [dead tower]→10
//   [war room]→7 [room list]→1 [assault]→9 [pvp type]→18 [dungeon type]→19
//   [all]→20 [blood system]→11 [tournament dungeon]→16 [despair tower]→12
//   未命中/默认→17（ORIG 默认分支 mov $0x11,%eax）
int getModuleType(const char* name)
{
    if (!name)
        return 17;
    static const struct { const char* name; int id; } table[] = {
        { "[pvp]", 4 },
        { "[fair pvp]", 5 },
        { "[dungeon]", 3 },
        { "[main game]", 3 },
        { "[dead tower]", 10 },
        { "[war room]", 7 },
        { "[room list]", 1 },
        { "[assault]", 9 },
        { "[pvp type]", 18 },
        { "[dungeon type]", 19 },
        { "[all]", 20 },
        { "[blood system]", 11 },
        { "[tournament dungeon]", 16 },
        { "[despair tower]", 12 },
    };
    static std::map<std::string, int> tbl;
    if (tbl.empty())
    {
        const int n = static_cast<int>(sizeof(table) / sizeof(table[0]));
        for (int i = 0; i < n; ++i)
            tbl.insert(std::map<std::string, int>::value_type(table[i].name, table[i].id));
    }
    std::map<std::string, int>::const_iterator it = tbl.find(name);
    if (it != tbl.end())
        return it->second;
    return 17;
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
