// ============================================================================
// df_game_r 还原 —— RestrictCategory + ServiceRestrictManager
// 权威实现（原 GlobalData.cpp 本地类迁移合并；报告 docs/class_func_reports/
// ServiceRestrictManager/）。
//   initialize 0x816e2b2 / savePolicy 0x816e8ce / loadPolicy 0x816e7d6 /
//   isRestricted(CUser*,Enum,int) 0x816e6b8 / isRestricted(Enum,int) 0x816e75e /
//   ctor 0x816e272。
// 策略类 InterfaceServicePolicy 的 vtable 8 槽（D1/D0 + 6 虚函数）与三个具体
// 策略类的具名符号见 RestrictPolicy.h（ORIG nm/objdump 实证）。
// ============================================================================

#include "ServiceRestrictManager.h"
#include "MsgQueueMgr.h"
#include "RestrictPolicy.h"           // NullPolicy/AuthMobileRestrictPolicy/CreateCharacterRestrictPolicy 声明
#include "MobileRestrict.h"           // MobileRestrict::enumToString（T @0x0816d463）
#include "MobileRestrict.h"           // MobileRestrict::enumToString（T @0x0816d463）
#include "CharacterRestrict.h"        // CharacterRestrict::enumToString（T @0x0816d472）
#include "SecurityRestrict.h"         // SecurityRestrict::enumToString（T @0x0816d481）

#include "CEnvironment.h"
#include "CStreamGuard.h"
#include "CUser.h"
#include "GlobalData.h"
#include "LogManager.h"
#include "TaiwanInternal.h"
#include "TaiwanQueries.h"  // LinkClosureD：Taiwan::SigLoadRestrictPolicy 权威
                            // （MoreTypes.h 0x100 占位已删）

#include <new>
#include <sstream>

// LinkClosureD：局部 Taiwan::GarenaAuthData 镜像已删（与 TaiwanQueries.h 权威
// 定义 ODR 冲突；getMobileAuth 权威实现在 TaiwanQueries.cpp）。

class CStreamGuard;

// ORIG static _ZL19setPolicyApplyValue...（0x0816d490）：策略字符串 '0'/'1' 解析
// 写入 policy vector（下标 1 起）；非法字符记日志置 0；越界即停。与
// SecurityRestrictPolicy.cpp 中副本同源（ORIG 同文件单定义，跨 TU 各一份）。
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

// ==================== 三个具体策略（ORIG 全局具名类，具名符号见 RestrictPolicy.h） ====================

// ---- NullPolicy（ORIG 0x0816d5aa 区，全部空/恒真）----
NullPolicy::NullPolicy() : InterfaceServicePolicy(RestrictCategory::ENUM_0)
{
    m_apply = false;          // ORIG 0816d5d1 movb $0,0x4
    m_svc.Init();             // ORIG 0816d5de call ServicePolicy::Init
}

NullPolicy::~NullPolicy()
{
}

bool NullPolicy::beginPolicy(std::string const&)
{
    return true;              // ORIG 0816d656 mov $1
}

void NullPolicy::endPolicy()
{
}                             // ORIG 0816d660 空

bool NullPolicy::isApply()
{
    return true;              // ORIG 0816d666 mov $1
}

bool NullPolicy::isApply(int)
{
    return true;              // ORIG 0816d670 mov $1
}

void NullPolicy::printRestrict()                                    // ORIG 0816d67a
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0x52, 9, true, true);
    trace("\tDummy Restrict");
}

void NullPolicy::getRestrictValue(Taiwan::SigLoadRestrictPolicy&)  // ORIG 0816d6c8 空
{
}

// ---- AuthMobileRestrictPolicy（ORIG 0x0816d6ce 区，code=1，34 条限制）----
AuthMobileRestrictPolicy::AuthMobileRestrictPolicy()
    : InterfaceServicePolicy(RestrictCategory::Enum_1)
{
    m_apply = false;          // ORIG 0816d6f5
    m_svc.Init();             // ORIG 0816d702
    for (int i = 0; i <= 0x21; ++i)   // ORIG 0816d72f cmpl $0x21/setle（34 条）
    {
        m_svc.push_back(static_cast<short>(1));   // ORIG push_back(short&&)
    }
}

AuthMobileRestrictPolicy::~AuthMobileRestrictPolicy()
{
}

bool AuthMobileRestrictPolicy::beginPolicy(std::string const& str)
{
    // ORIG 读 this+0x8（m_code）传 setPolicyApplyValue。
    setPolicyApplyValue(static_cast<RestrictCategory::Enum>(m_code), m_svc, str);
    return true;
}

void AuthMobileRestrictPolicy::endPolicy()
{
    m_apply = false;          // ORIG 0816d7d8
}

bool AuthMobileRestrictPolicy::isApply()
{
    return m_apply;           // ORIG 0816d7e4 movzbl
}

bool AuthMobileRestrictPolicy::isApply(int param)                   // ORIG 0816d7f0
{
    if (param > 0x21 || param < 0)
    {
        LogManager::logFormat(1, "localtaiwan/System/ServiceRestrictManager.cpp",
                              __PRETTY_FUNCTION__, 0x84,
                              "[Taiwan, Restrict] Invalid AuthMobileRestrictPolicy Parameter. End:%d, Param:%d",
                              0x22, param);
        return false;
    }
    return m_svc[param] == 1;
}

void AuthMobileRestrictPolicy::printRestrict()                      // ORIG 0816d866
{
    unsigned int i;
    cMyTrace trace1(__PRETTY_FUNCTION__, 0x8d, 9, true, true);
    trace1("\tMobile Auth Begin");
    for (i = 0; i < m_svc.size(); ++i)
    {
        // P37 链式临时：实参右→左求值（m_svc[i]→esi、enumToString→ebx）后构造。
        cMyTrace(__PRETTY_FUNCTION__, 0x90, 9, true, true)(
            "\t- %s : %d",
            MobileRestrict::enumToString(static_cast<MobileRestrict::Enum>(i)),
            static_cast<int>(m_svc[i]));
    }
    cMyTrace trace3(__PRETTY_FUNCTION__, 0x94, 9, true, true);
    trace3("\tMobile Auth End");
}

void AuthMobileRestrictPolicy::getRestrictValue(Taiwan::SigLoadRestrictPolicy& policy)  // 0816d9a0
{
    policy.m_restrictValue = getCode();   // ORIG call 816ee12
    policy.m_count = 0;
    for (unsigned int i = 1; i < m_svc.size(); ++i)
    {
        // ORIG 0816d9cc：enumToString 实参内联（无命名局部；P24）。
        strncpy(policy.m_data[i],
                MobileRestrict::enumToString(static_cast<MobileRestrict::Enum>(i)), 0x2d);
        policy.m_extra[i] = static_cast<unsigned char>(m_svc[i]);
        ++policy.m_count;
    }
}

// ---- CreateCharacterRestrictPolicy（ORIG 0x0816ddd2 区，code=2，3 条限制）----
CreateCharacterRestrictPolicy::CreateCharacterRestrictPolicy()
    : InterfaceServicePolicy(RestrictCategory::Enum_2)
{
    m_apply = false;          // ORIG 0816de0d
    m_svc.Init();             // ORIG 0816de06
    for (int i = 0; i <= 2; ++i)      // ORIG 0816de34 cmpl $0x2/setle（3 条）
    {
        m_svc.push_back(static_cast<short>(i));   // ORIG push_back(short&&)
    }
}

CreateCharacterRestrictPolicy::~CreateCharacterRestrictPolicy()
{
}

bool CreateCharacterRestrictPolicy::beginPolicy(std::string const& str)
{
    // ORIG 读 this+0x8（m_code）传 setPolicyApplyValue。
    setPolicyApplyValue(static_cast<RestrictCategory::Enum>(m_code), m_svc, str);
    return true;
}

void CreateCharacterRestrictPolicy::endPolicy()
{
    m_apply = false;          // ORIG 0816dede
}

bool CreateCharacterRestrictPolicy::isApply()
{
    return m_apply;           // ORIG 0816deea movzbl
}

bool CreateCharacterRestrictPolicy::isApply(int param)              // ORIG 0816def6
{
    if (param > 2 || param < 0)
    {
        LogManager::logFormat(1, "localtaiwan/System/ServiceRestrictManager.cpp",
                              __PRETTY_FUNCTION__, 0x11c,
                              "[Taiwan, Restrict] Invalid CreateCharacterRestrictPolicy Parameter. End:%d, Param:%d",
                              3, param);
        return false;
    }
    return m_svc[param] == 1;
}

void CreateCharacterRestrictPolicy::printRestrict()                 // ORIG 0816df6c
{
    cMyTrace trace1(__PRETTY_FUNCTION__, 0x125, 9, true, true);
    trace1("\tCreate Character Begin");
    for (unsigned int i = 0; i < m_svc.size(); ++i)
    {
        // P37 链式临时：实参右→左求值（m_svc[i]→esi、enumToString→ebx）后构造。
        cMyTrace(__PRETTY_FUNCTION__, 0x128, 9, true, true)(
            "\t- %s : %d",
            CharacterRestrict::enumToString(static_cast<CharacterRestrict::Enum>(i)),
            static_cast<int>(m_svc[i]));
    }
    cMyTrace trace3(__PRETTY_FUNCTION__, 0x12c, 9, true, true);
    trace3("\tCreate Character End");
}

void CreateCharacterRestrictPolicy::getRestrictValue(Taiwan::SigLoadRestrictPolicy&)  // 0816e0a6 空
{
}

// ---- ctor / dtor（ORIG 0x816e272：map 构造 + 两标志字节清零）----

// ---- ctor / dtor（ORIG 0x816e272：map 构造 + 两标志字节清零）----
ServiceRestrictManager::ServiceRestrictManager() : m_policyMap(), m_flag18(0), m_flag19(0)
{
}

// ORIG 0x0816e2b2 旁 D1：仅 call map::~map（不 delete 策略对象，ORIG 原样）。
ServiceRestrictManager::~ServiceRestrictManager()
{
}

// ORIG 0x0816e2b2：channel_no==1 → 置 +0x19=1；创建 4 个策略对象注册进 map。
bool ServiceRestrictManager::initialize()
{
    // 复用单指针变量（ORIG -0x1c(%ebp) 单槽，函数开头预置 0）。
    InterfaceServicePolicy* p = 0;
    if (G_CEnvironment()->get_channel_no() == 1)
    {
        m_flag19 = 1;
    }
    // ORIG 0x0816e2e2：每策略独立 new（普通 _Znwj 0x18）→ 判空 →
    // 独立 logFormat（行号 0x169/0x171/0x17a/0x183）→ return false。
    p = new NullPolicy;
    if (p == 0)
    {
        LogManager::logFormat(1, "localtaiwan/System/ServiceRestrictManager.cpp",
                              __PRETTY_FUNCTION__, 0x169,
                              "[Taiwan, Restrict] Fail create NullPolicy.");
        return false;
    }
    m_policyMap[(RestrictCategory::Enum)p->getCode()] = p;
    p = new AuthMobileRestrictPolicy;
    if (p == 0)
    {
        LogManager::logFormat(1, "localtaiwan/System/ServiceRestrictManager.cpp",
                              __PRETTY_FUNCTION__, 0x171,
                              "[Taiwan, Restrict] Fail create AuthMobileRestrictPolicy.");
        return false;
    }
    m_policyMap[(RestrictCategory::Enum)p->getCode()] = p;
    p = new CreateCharacterRestrictPolicy;
    if (p == 0)
    {
        LogManager::logFormat(1, "localtaiwan/System/ServiceRestrictManager.cpp",
                              __PRETTY_FUNCTION__, 0x17a,
                              "[Taiwan, Restrict] Fail create CreateCharacterRestrictPolicy.");
        return false;
    }
    m_policyMap[(RestrictCategory::Enum)p->getCode()] = p;
    p = new SecurityRestrictPolicy;
    if (p == 0)
    {
        LogManager::logFormat(1, "localtaiwan/System/ServiceRestrictManager.cpp",
                              __PRETTY_FUNCTION__, 0x183,
                              "[Taiwan, Restrict] Fail create SecurityRestrictPolicy.");
        return false;
    }
    m_policyMap[(RestrictCategory::Enum)p->getCode()] = p;
    return true;
}

// ORIG 0x0816e8ce：+0x19==1 时遍历 map，每条策略经 SigSaveRestrictPolicy 流序列化
// 后入 DB 队列。策略 save() 为虚调用（策略数据未还原，推断）。
void ServiceRestrictManager::savePolicy()
{
    if (m_flag19 != 1)
    {
        return;
    }
    for (std::map<RestrictCategory::Enum, InterfaceServicePolicy*>::iterator it = m_policyMap.begin();
         it != m_policyMap.end(); ++it)
    {
        if (it->second == 0)
        {
            continue;
        }
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                               "localtaiwan/System/ServiceRestrictManager.cpp", 0x1de), true);
        Taiwan::internal_stream(guard, TaiwanInternalPack::T(2), 0xffffffff);
        Taiwan::SigLoadRestrictPolicy* sig =
            guard->GetInBuffer<Taiwan::SigLoadRestrictPolicy>();
        if (sig == 0)
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x1e3, 5);
            t("[Taiwan, Restrict] SigSaveRestrictPolicy is null.");
            return;
        }
        // ORIG 0x0816e9a6：GetInBuffer 后先 sig->init() 清零（见 TaiwanQueries.cpp
        // ORIG W 0x0815dda8），再虚调用 vtable+0x1c（槽7）= getRestrictValue(sig)。
        sig->init();
        it->second->getRestrictValue(*sig);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}

// ORIG 0x0816e7d6：SigLoadRestrictPolicy 请求（internal_stream(1,-1)）入 DB 队列。
void ServiceRestrictManager::loadPolicy()
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localtaiwan/System/ServiceRestrictManager.cpp", 0x1c5), true);
    Taiwan::internal_stream(guard, TaiwanInternalPack::T(1), 0xffffffff);
    Taiwan::SigLoadRestrictPolicy* sig =
        guard->GetInBuffer<Taiwan::SigLoadRestrictPolicy>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localtaiwan/System/ServiceRestrictManager.cpp",
                              __PRETTY_FUNCTION__, 0x1cc,
                              "[Taiwan, Restrict] SigLoadRestrictPolicy is null.");
    }
    else
    {
        // ORIG 0x0816e8ab：GetInBuffer 后先 sig->init() 清零再入 DB 队列。
        sig->init();
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}

// ORIG 0x0816e6b8：user 空→false；已开 Garena 手机认证→false；map 无该类别→true；
// 否则调策略虚函数（vtable+0x14）。
bool ServiceRestrictManager::isRestricted(CUser* user,
                                          RestrictCategory::Enum category, int v)
{
    if (user == 0)
        return false;
    Taiwan::GarenaAuthData* auth =
        reinterpret_cast<Taiwan::GarenaAuthData*>(user->getGarenaAuthData());
    if (auth->getMobileAuth())
        return false;
    std::map<RestrictCategory::Enum, InterfaceServicePolicy*>::iterator it =
        m_policyMap.find(category);
    if (it == m_policyMap.end())
        return true;
    // ORIG 0x0816e7ca：虚调用 vtable+0x14（槽5）= isApply(int)。
    return it->second->isApply(v);
}

// ORIG 0x0816e75e（WarField 用）：map 无该类别→true；否则策略虚调用。
bool ServiceRestrictManager::isRestricted(RestrictCategory::Enum category, int v)
{
    std::map<RestrictCategory::Enum, InterfaceServicePolicy*>::iterator it =
        m_policyMap.find(category);
    if (it == m_policyMap.end())
        return true;
    // ORIG 0x0816e7ca：虚调用 vtable+0x14（槽5）= isApply(int)。
    return it->second->isApply(v);
}

// ORIG 0x0816e54a：find 命中则策略 beginPolicy(str)；随后 printPolicy + updatePolicy。
void ServiceRestrictManager::beginPolicy(RestrictCategory::Enum category,
                                         std::string const& str)
{
    std::map<RestrictCategory::Enum, InterfaceServicePolicy*>::iterator it =
        m_policyMap.find(category);
    if (it != m_policyMap.end())
    {
        // ORIG 0x0816e5a7：虚调用 vtable+0x8（槽2）= beginPolicy(string)。
        it->second->beginPolicy(str);
    }
    printPolicy(category);
    updatePolicy(category);
}

// ORIG 0x0816e5de：find 命中则策略 endPolicy()（虚槽 +0xc）。
void ServiceRestrictManager::endPolicy(RestrictCategory::Enum category)
{
    std::map<RestrictCategory::Enum, InterfaceServicePolicy*>::iterator it =
        m_policyMap.find(category);
    if (it != m_policyMap.end())
    {
        it->second->endPolicy();
    }
}

// ORIG 0x0816e648：map 无该类别→true；否则策略 isApply()（虚槽 +0x10）。
bool ServiceRestrictManager::isApply(RestrictCategory::Enum category)
{
    std::map<RestrictCategory::Enum, InterfaceServicePolicy*>::iterator it =
        m_policyMap.find(category);
    if (it != m_policyMap.end())
        return it->second->isApply();
    return true;
}

// ORIG 0x0816ea6a：trace 头 + 命中则策略 printRestrict()（虚槽 +0x18），
// 未命中打 End 行。
void ServiceRestrictManager::printPolicy(RestrictCategory::Enum category)
{
    cMyTrace trace1(__PRETTY_FUNCTION__,
                    0x1ef, 9, true, true);
    trace1("[!] Service Restrict List ... ");
    std::map<RestrictCategory::Enum, InterfaceServicePolicy*>::iterator it =
        m_policyMap.find(category);
    if (it != m_policyMap.end())
    {
        it->second->printRestrict();
    }
    else
    {
        cMyTrace trace2(__PRETTY_FUNCTION__,
                        0x1f3, 9, true, true);
        trace2("[!] Service Restrict List ... End");
    }
}

// ORIG 0x0816eb62：find 命中则经 SigSaveRestrictPolicy 流把策略当前值写回 DB。
void ServiceRestrictManager::updatePolicy(RestrictCategory::Enum category)
{
    std::map<RestrictCategory::Enum, InterfaceServicePolicy*>::iterator it =
        m_policyMap.find(category);
    if (it != m_policyMap.end())
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                               "localtaiwan/System/ServiceRestrictManager.cpp", 0x1fd), true);
        Taiwan::internal_stream(guard, TaiwanInternalPack::T(2), 0xffffffff);
        Taiwan::SigLoadRestrictPolicy* sig =
            guard->GetInBuffer<Taiwan::SigLoadRestrictPolicy>();
        if (sig == 0)
        {
            cMyTrace t(__PRETTY_FUNCTION__,
                       0x202, 5);
            t("[Taiwan, Restrict] SigSaveRestrictPolicy is null.");
        }
        else
        {
            // ORIG 0x0816ec69：虚调用 vtable+0x1c（槽7）= getRestrictValue(sig)。
            sig->init();
            it->second->getRestrictValue(*sig);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        }
    }
}

// ORIG 0x0816e0ac：拼接 Mobile(0..0x21) 与 Character(0..2) 限制串到 ostringstream
// （str() 结果丢弃，原样保留）。
void ServiceRestrictManager::serviceRestrictStringQuery()
{
    std::ostringstream oss;
    for (int i = 0; i <= 0x21; ++i)
    {
        oss << "(" << 1 << "," << i << ","
            << MobileRestrict::enumToString(static_cast<MobileRestrict::Enum>(i)) << ")";
    }
    for (int i = 0; i <= 2; ++i)
    {
        oss << "(" << 2 << "," << i << ","
            << CharacterRestrict::enumToString(static_cast<CharacterRestrict::Enum>(i)) << ")";
    }
    oss.str();
}
