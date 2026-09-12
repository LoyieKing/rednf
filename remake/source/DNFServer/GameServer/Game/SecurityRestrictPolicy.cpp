#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 SecurityRestrictPolicy/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// SecurityRestrictPolicy —— Taiwan 服务限制策略实现（ORIG T @0x0816da50 区，9 符号）
// 报告已读：docs/class_func_reports/SecurityRestrictPolicy/{README, SecurityRestrictPolicy,
// beginPolicy, endPolicy, getRestrictValue, isApply, isApply_0816db72, printRestrict,
// ~SecurityRestrictPolicy, ~SecurityRestrictPolicy_0816db10}.md
#include "SecurityRestrictPolicy.h"

#include "SecurityRestrict.h"   // SecurityRestrict::enumToString（T @0x0816d481）
#include "TaiwanQueries.h"      // Taiwan::SigLoadRestrictPolicy 完整类型
#include "LogManager.h"         // logFormat（0x08ad3c0a）
#include "GameTypes.h"          // cMyTrace（ctor 0x0854f746 / operator() 0x0854f788）
#include "MobileRestrict.h"
#include "CharacterRestrict.h"

#include <cstring>   // strncpy
#include <string>

// ORIG static _ZL19setPolicyApplyValueN16RestrictCategory4EnumER13ServicePolicyRKSs
//      @0x0816d490（objdump 0x0816d490-0x0816d5a9 转写）。
// 语义：字符串逐字符 '0'/'1' → 值 0/1，从下标 1 起写入策略 vector；
//       非法字符记日志并置 0；超出 vector 长度即停止；空串直接返回。
static void setPolicyApplyValue(RestrictCategory::Enum code, ServicePolicy& policy,
                                std::string const& str)
{
    if (str.empty())
    {
        return;
    }
    unsigned int idx = 1;
    unsigned int size = policy.size();
    std::string::const_iterator it = str.begin();
    while (it != str.end())
    {
        int v = static_cast<signed char>(*it) - 0x30;
        if (v > 1 || v < 0)
        {
            LogManager::logFormat(1, "localtaiwan/System/ServiceRestrictManager.cpp",
                                  __PRETTY_FUNCTION__,
                                  0x22,
                                  "[Taiwan, Restrict] Invalid server value. restrict:%d,%d,%d",
                                  code, idx, v);
            v = 0;
        }
        if (idx >= size)
        {
            break;
        }
        policy[idx] = static_cast<short>(v);
        ++it;
        ++idx;
    }
}

#include "SecurityRestrictPolicy/SecurityRestrictPolicy.inc"


#include "SecurityRestrictPolicy/dtor_SecurityRestrictPolicy.inc"


#include "SecurityRestrictPolicy/beginPolicy.inc"


#include "SecurityRestrictPolicy/endPolicy.inc"


#include "SecurityRestrictPolicy/isApply.inc"


#include "SecurityRestrictPolicy/isApply_1.inc"


#include "SecurityRestrictPolicy/printRestrict.inc"


#include "SecurityRestrictPolicy/getRestrictValue.inc"


// ORIG d @0x0937015c（0xc=3 项）raw 逐项实证（段感知：ORIG .data off=0x1325020）：
// [0]=RestrictBegin / [1]=DropRate / [2]=RestrictEnd（0x08e1de5c 区串序逐个解引用核对）。
// 【修正 b28/R18obj2】原写 {RestrictBegin, RestrictEnd, DropRate} 的 [1]/[2] 与 ORIG 相反：
// getRestrictValue 以 i=1..2 索引写入 SigLoadRestrictPolicy::m_data[i]（协议载荷）⇒ 真分叉。
// ORIG 侧为内部链接（mangled 带 L）；类外定义不允许 static，故为外部链接——
// 仅链接可见性差异，值/索引一致，按结构噪音豁免。
char const* SecurityRestrict::enumStringSecurity[3] = {
    "RestrictBegin", "DropRate", "RestrictEnd"
};
char const* MobileRestrict::enumStringMobiles[35] = {
    "RestrictBegin", "DeleteItem", "DropGold", "UseNpc", "UseMail", "UseAuction",
    "UseTrade", "UseUpgrade", "UseEnchant", "UseCompound", "UseDisjoint",
    "UsePrivateStore", "UseStackable", "UseBindSphere", "UseSeal",
    "UseRandomOptionChange", "UseRandomOptionReGeneration", "UseCeraShop",
    "UseRandomBox", "UseExportJob", "UseDisjointAvatar", "UseEmblemCompound",
    "RecoverStamina", "DeleteCharacter", "AccountCargo", "AccountUpgrade",
    "AccountMoveGold", "AccountMoveItem", "GuildCreate", "GuildLevelUp",
    "GuildSkillUp", "GuildBreak", "CreateCharacter", "LoginChannel", "RestrictEnd"
};
// ORIG d @0x0937014c（0x10=4 项）raw 逐项实证：
// [0]=RestrictBegin / [1]=RestrictOnDay / [2]=RestrictIP / [3]=RestrictEnd
// 【修正 b28/R18obj2】原写 {RestrictBegin, RestrictEnd, RestrictOnDay, RestrictIP} 的 [1..3]
// 与 ORIG 轮换错位：serviceRestrictStringQuery/printRestrict 以 i=0..2 索引 ⇒ 字符串分叉。
// 语义：索引即 Enum 值（消费点 cast<Enum>(i) 遍历）。
char const* CharacterRestrict::enumStringCharacters[4] = {
    "RestrictBegin", "RestrictOnDay", "RestrictIP", "RestrictEnd"
};

#include "SecurityRestrictPolicy/enumToString.inc"


#include "SecurityRestrictPolicy/enumToString_1.inc"


#include "SecurityRestrictPolicy/enumToString_2.inc"

