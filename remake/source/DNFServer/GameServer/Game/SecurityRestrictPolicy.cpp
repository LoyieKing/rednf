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
                                  "void setPolicyApplyValue(RestrictCategory::Enum, ServicePolicy&, std::string const&)",
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

// ORIG 0x0816da50：基类 ctor(Enum_3) + 清 m_apply + ServicePolicy::Init +
// push_back(1) × 2（setle 循环 i<=1）。
SecurityRestrictPolicy::SecurityRestrictPolicy()
    : InterfaceServicePolicy(RestrictCategory::Enum_3)
{
    m_apply = false;
    m_svc.Init();
    for (int i = 0; i <= 1; ++i)
    {
        m_svc.push_back(static_cast<short>(1));   // ORIG push_back(short&&)
    }
}

// ORIG 0x0816dae0：设 vptr + 调基类析构 + __in_chrg 检查（空体，编译器生成同形）。
SecurityRestrictPolicy::~SecurityRestrictPolicy()
{
}

// ORIG 0x0816db2e：setPolicyApplyValue(m_code, m_svc, str)；返回 1。
bool SecurityRestrictPolicy::beginPolicy(std::string const& str)
{
    setPolicyApplyValue(static_cast<RestrictCategory::Enum>(m_code), m_svc, str);
    return true;
}

// ORIG 0x0816db5a：m_apply = false。
void SecurityRestrictPolicy::endPolicy()
{
    m_apply = false;
}

// ORIG 0x0816db66：直返 m_apply（movzbl +0x4）。
bool SecurityRestrictPolicy::isApply()
{
    return m_apply;
}

// ORIG 0x0816db72：param ∈ [0,1] 时返回 m_svc[param] == 1；越界记日志返 false。
bool SecurityRestrictPolicy::isApply(int param)
{
    if (param > 1 || param < 0)
    {
        LogManager::logFormat(1, "localtaiwan/System/ServiceRestrictManager.cpp",
                              __PRETTY_FUNCTION__, 0xcd,
                              "[Taiwan, Restrict] Invalid SecurityRestrictPolicy Parameter. End:%d, Param:%d",
                              2, param);
        return false;
    }
    return m_svc[param] == 1;
}

// ORIG 0x0816dbe8：cMyTrace 三段 + 循环打印每条限制。
void SecurityRestrictPolicy::printRestrict()
{
    // b28c-L20: AE — 三处 cMyTrace 全改「构造即调用」全表达式临时链
    // `cMyTrace(f,l,v)(fmt,...)`（P221/P37）。原 b26「[N82] 槽分配方向跨版本相反、
    // ndiff 12 封顶」判词作废（同型复现另见 AuthMobile/CreateCharacter 两件与
    // 他 lane 的 CUdpHandler::SendToServer、MySQL::init_db_handle）：具名对象形 4.4 按「函数级先声明浅、块内更深」排
    // （trace1 -0x1c/trace3 -0x2c/trace2 -0x3c）与 ORIG(4.1) 的
    // trace1 -0x3c/trace2 -0x2c/trace3 -0x1c 相反 ⇒ ndiff 12；改匿名临时后
    // 4.4 槽落位翻正，orig=our=71、ndiff 0。
    // 循环体取「实参内联、零具名局部」形（同 AuthMobile/CreateCharacter 两件）：
    // 原 `register int s` / `register const char* name` 两枚裸 register 经 b28c-L20
    // 对照实测**不必要**（去掉后仍 71=71 ndiff 0，identical/ae 计数不变）——s→%esi、
    // name→%ebx 的跨 ctor 保活由 4.4 对表达式临时值的自然 callee-saved 升格产生，
    // 无需 register 关键字（参见 lane20A 负例：裸 register 会强制占用 ebx 并撑帧，
    // SetServerMessage ndiff 3→13）。
    cMyTrace(__PRETTY_FUNCTION__, 0xd7, 9, true, true)("\tSecurity Restrict Begin");
    for (unsigned int i = 0; i < m_svc.size(); ++i)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xda, 9, true, true)(
            "\t- %s : %d",
            SecurityRestrict::enumToString(static_cast<SecurityRestrict::Enum>(i)),
            static_cast<int>(m_svc[i]));
    }
    cMyTrace(__PRETTY_FUNCTION__, 0xde, 9, true, true)("\tSecurity Restrict End");
}

// ORIG 0x0816dd22：code/count 头 + 每条 name[45]（strncpy）+ flag 字节 + count 自增。
void SecurityRestrictPolicy::getRestrictValue(Taiwan::SigLoadRestrictPolicy& policy)
{
    // b28c-L20: AE — 首字段 store 装载序由 LHS-first 翻正为 ORIG 的 RHS-first+中转
    // （`call getCode; mov %eax,%edx; mov 0xc(%ebp),%eax; mov %edx,(%eax)`）：
    // InterfaceServicePolicy::getCode() 返回类型 int→RestrictCategory::Enum（头内，
    // mangled/函数体 asm 不变）。P211 双向开关：int 字段←int 返回恒 LHS 先；
    // 返回类型与字段异型才产生 ORIG 的中转 3 条形（探针 /tmp/l20b/probe2.cpp
    // f_int/f_enum；源侧加 cast 无效 = probe3.cpp f_cast_enum_field 实测仍 LHS 先）。
    // 反证字段本身为 int：Taiwan::SigLoadRestrictPolicy::set @0x0815de02 形参 int 写
    // 同字段为 LHS 先无中转。ndiff 5→0（orig=54 our=53→54=54）。
    policy.m_restrictValue = getCode();
    policy.m_count = 0;
    for (unsigned int i = 1; i < m_svc.size(); ++i)
    {
        // P44 实参即时求值：enumToString 结果不落栈槽（ORIG 直接 eax 传参）。
        strncpy(policy.m_data[i],
                SecurityRestrict::enumToString(static_cast<SecurityRestrict::Enum>(i)),
                0x2d);
        policy.m_extra[i] = static_cast<unsigned char>(m_svc[i]);
        ++policy.m_count;
    }
}

// ORIG d @0x0937015c（0xc=3 项）：{RestrictBegin, RestrictEnd, DropRate}
// ORIG 侧为内部链接（mangled 带 L）；类外定义不允许 static，故为外部链接——
// 仅链接可见性差异，值/索引一致，按结构噪音豁免。
char const* SecurityRestrict::enumStringSecurity[3] = {
    "RestrictBegin", "RestrictEnd", "DropRate"
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
char const* CharacterRestrict::enumStringCharacters[4] = {
    "RestrictBegin", "RestrictEnd", "RestrictOnDay", "RestrictIP"
};

char const* SecurityRestrict::enumToString(Enum e)
{
    // ORIG 0x0816d481：无边界检查，直接按 Enum 查表（同 MobileRestrict 形态）。
    return enumStringSecurity[e];
}

char const* MobileRestrict::enumToString(Enum e)
{
    // ORIG 0x0816d463：无边界检查，直接按 Enum 查表（表 35 项）。
    return enumStringMobiles[e];
}

char const* CharacterRestrict::enumToString(Enum e)
{
    // ORIG 0x0816d472：无边界检查，直接按 Enum 查表（同 MobileRestrict 形态）。
    return enumStringCharacters[e];
}
