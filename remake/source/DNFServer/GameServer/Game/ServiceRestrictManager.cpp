// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ServiceRestrictManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
#include "ServiceRestrictManager/NullPolicy.inc"
{
    m_apply = false;          // ORIG 0816d5d1 movb $0,0x4
    m_svc.Init();             // ORIG 0816d5de call ServicePolicy::Init
}

#include "ServiceRestrictManager/dtor_NullPolicy.inc"
{
}

#include "ServiceRestrictManager/beginPolicy.inc"
{
    return true;              // ORIG 0816d656 mov $1
}

#include "ServiceRestrictManager/endPolicy.inc"
{
}                             // ORIG 0816d660 空

#include "ServiceRestrictManager/isApply.inc"
{
    return true;              // ORIG 0816d666 mov $1
}

#include "ServiceRestrictManager/isApply_1.inc"
{
    return true;              // ORIG 0816d670 mov $1
}

#include "ServiceRestrictManager/printRestrict.inc"
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0x52, 9, true, true);
    trace("\tDummy Restrict");
}

#include "ServiceRestrictManager/getRestrictValue.inc"
{
}

// ---- AuthMobileRestrictPolicy（ORIG 0x0816d6ce 区，code=1，34 条限制）----
#include "ServiceRestrictManager/AuthMobileRestrictPolicy.inc"
    : InterfaceServicePolicy(RestrictCategory::Enum_1)
{
    m_apply = false;          // ORIG 0816d6f5
    m_svc.Init();             // ORIG 0816d702
    for (int i = 0; i <= 0x21; ++i)   // ORIG 0816d72f cmpl $0x21/setle（34 条）
    {
        m_svc.push_back(static_cast<short>(1));   // ORIG push_back(short&&)
    }
}

#include "ServiceRestrictManager/dtor_AuthMobileRestrictPolicy.inc"
{
}

#include "ServiceRestrictManager/beginPolicy_1.inc"
{
    // ORIG 读 this+0x8（m_code）传 setPolicyApplyValue。
    setPolicyApplyValue(static_cast<RestrictCategory::Enum>(m_code), m_svc, str);
    return true;
}

#include "ServiceRestrictManager/endPolicy_1.inc"
{
    m_apply = false;          // ORIG 0816d7d8
}

#include "ServiceRestrictManager/isApply_2.inc"
{
    return m_apply;           // ORIG 0816d7e4 movzbl
}

#include "ServiceRestrictManager/isApply_3.inc"
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

#include "ServiceRestrictManager/printRestrict_1.inc"
{
    // b28c-L20: AE — 同 SecurityRestrictPolicy::printRestrict：三处 cMyTrace 改
    // 全表达式临时链（P221/P37）后 4.4 槽落位翻正（ORIG trace1 -0x3c/trace2 -0x2c/
    // trace3 -0x1c），b26「[N82] ndiff 12 封顶」判词作废；71=71 ndiff 0。
    unsigned int i;
    cMyTrace(__PRETTY_FUNCTION__, 0x8d, 9, true, true)("\tMobile Auth Begin");
    for (i = 0; i < m_svc.size(); ++i)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x90, 9, true, true)(
            "\t- %s : %d",
            MobileRestrict::enumToString(static_cast<MobileRestrict::Enum>(i)),
            static_cast<int>(m_svc[i]));
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x94, 9, true, true)("\tMobile Auth End");
}

#include "ServiceRestrictManager/getRestrictValue_1.inc"
{
    // b28c-L20: AE — 首字段 store 翻正同 SecurityRestrictPolicy::getRestrictValue：
    // getCode() 返回类型 int→RestrictCategory::Enum（P211 异型 ⇒ RHS 先+中转，
    // 探针 /tmp/l20b/probe2.cpp）。orig=54 our=53 gap=1 → 54=54 ndiff 0。
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
#include "ServiceRestrictManager/CreateCharacterRestrictPolicy.inc"
    : InterfaceServicePolicy(RestrictCategory::Enum_2)
{
    m_apply = false;          // ORIG 0816de0d
    m_svc.Init();             // ORIG 0816de06
    for (int i = 0; i <= 2; ++i)      // ORIG 0816de34 cmpl $0x2/setle（3 条）
    {
        m_svc.push_back(static_cast<short>(i));   // ORIG push_back(short&&)
    }
}

#include "ServiceRestrictManager/dtor_CreateCharacterRestrictPolicy.inc"
{
}

#include "ServiceRestrictManager/beginPolicy_2.inc"
{
    // ORIG 读 this+0x8（m_code）传 setPolicyApplyValue。
    setPolicyApplyValue(static_cast<RestrictCategory::Enum>(m_code), m_svc, str);
    return true;
}

#include "ServiceRestrictManager/endPolicy_2.inc"
{
    m_apply = false;          // ORIG 0816dede
}

#include "ServiceRestrictManager/isApply_4.inc"
{
    return m_apply;           // ORIG 0816deea movzbl
}

#include "ServiceRestrictManager/isApply_5.inc"
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

#include "ServiceRestrictManager/printRestrict_2.inc"
{
    // b28c-L20: AE — 同 SecurityRestrictPolicy::printRestrict 修法（P221/P37 全表达式
    // 临时链翻正 4.4 的 cMyTrace 槽落位，ORIG trace1 -0x3c/trace2 -0x2c/trace3 -0x1c）；
    // b26「[N82] ndiff 12 封顶」判词作废。
    cMyTrace(__PRETTY_FUNCTION__, 0x125, 9, true, true)("\tCreate Character Begin");
    for (unsigned int i = 0; i < m_svc.size(); ++i)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x128, 9, true, true)(
            "\t- %s : %d",
            CharacterRestrict::enumToString(static_cast<CharacterRestrict::Enum>(i)),
            static_cast<int>(m_svc[i]));
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x12c, 9, true, true)("\tCreate Character End");
}

#include "ServiceRestrictManager/getRestrictValue_2.inc"
{
}

// ---- ctor / dtor（ORIG 0x816e272：map 构造 + 两标志字节清零）----

// ---- ctor / dtor（ORIG 0x816e272：map 构造 + 两标志字节清零）----
#include "ServiceRestrictManager/ServiceRestrictManager.inc"
{
}

// ORIG 0x0816e2b2 旁 D1：仅 call map::~map（不 delete 策略对象，ORIG 原样）。
#include "ServiceRestrictManager/dtor_ServiceRestrictManager.inc"
{
}

// ORIG 0x0816e2b2：channel_no==1 → 置 +0x19=1；创建 4 个策略对象注册进 map。
#include "ServiceRestrictManager/initialize.inc"
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
#include "ServiceRestrictManager/savePolicy.inc"
{
    // ORIG 0x0816e8dd：movzbl 0x19(%eax); xor $1; test %al; jne 尾 ⇒ 源码取反式。
    if (!m_flag19)
    {
        return;
    }
    for (std::map<RestrictCategory::Enum, InterfaceServicePolicy*>::iterator it = m_policyMap.begin();
         it != m_policyMap.end(); ++it)
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                               "localtaiwan/System/ServiceRestrictManager.cpp", 0x1de), true);
        Taiwan::internal_stream(guard, TaiwanInternalPack::T(2), 0xffffffff);
        Taiwan::SigLoadRestrictPolicy* sig =
            guard->GetInBuffer<Taiwan::SigLoadRestrictPolicy>();
        if (sig == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1e3, 5)(
                "[Taiwan, Restrict] SigSaveRestrictPolicy is null.");
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
#include "ServiceRestrictManager/loadPolicy.inc"
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
#include "ServiceRestrictManager/isRestricted.inc"
                                          RestrictCategory::Enum category, int v)
{
    if (user == 0)
        return false;
    if (reinterpret_cast<Taiwan::GarenaAuthData*>(user->getGarenaAuthData())->getMobileAuth())
        return false;
    std::map<RestrictCategory::Enum, InterfaceServicePolicy*>::iterator it =
        m_policyMap.find(category);
    if (it != m_policyMap.end())
    {
        // ORIG 0x0816e7ca：虚调用 vtable+0x14（槽5）= isApply(int)。
        return it->second->isApply(v);
    }
    return true;
}

// ORIG 0x0816e75e（WarField 用）：map 无该类别→true；否则策略虚调用。
#include "ServiceRestrictManager/isRestricted_1.inc"
{
    std::map<RestrictCategory::Enum, InterfaceServicePolicy*>::iterator it =
        m_policyMap.find(category);
    // [b28c-L15 P220] ORIG 0x0816e7a9：`test %al; je 尾 mov$1` + fall-through 虚调用
    //   ⇒ 正极性迭代器比较（同 isApply 形），false 侧归共享尾。
    if (it != m_policyMap.end())
    {
        // ORIG 0x0816e7ca：虚调用 vtable+0x14（槽5）= isApply(int)。
        return it->second->isApply(v);
    }
    return true;
}

// ORIG 0x0816e54a：find 命中则策略 beginPolicy(str)；随后 printPolicy + updatePolicy。
#include "ServiceRestrictManager/beginPolicy_3.inc"
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
#include "ServiceRestrictManager/endPolicy_3.inc"
{
    std::map<RestrictCategory::Enum, InterfaceServicePolicy*>::iterator it =
        m_policyMap.find(category);
    if (it != m_policyMap.end())
    {
        it->second->endPolicy();
    }
}

// ORIG 0x0816e648：map 无该类别→true；否则策略 isApply()（虚槽 +0x10）。
#include "ServiceRestrictManager/isApply_6.inc"
{
    std::map<RestrictCategory::Enum, InterfaceServicePolicy*>::iterator it =
        m_policyMap.find(category);
    if (it != m_policyMap.end())
        return it->second->isApply();
    return true;
}

// ORIG 0x0816ea6a：trace 头 + 命中则策略 printRestrict()（虚槽 +0x18），
// 未命中打 End 行。
#include "ServiceRestrictManager/printPolicy.inc"
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
        cMyTrace(__PRETTY_FUNCTION__,
                 0x1f3, 9, true, true)("[!] Service Restrict List ... End");
    }
}

// ORIG 0x0816eb62：find 命中则经 SigSaveRestrictPolicy 流把策略当前值写回 DB。
#include "ServiceRestrictManager/updatePolicy.inc"
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
            cMyTrace(__PRETTY_FUNCTION__,
                     0x202, 5)("[Taiwan, Restrict] SigSaveRestrictPolicy is null.");
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

// ORIG 0x0816e0ac：static 成员（P245/P281：首参 0x8(%ebp)=sret、ret $0x4、全程无 this），
// 返回 std::string（sret）。拼接 Mobile(0..0x21) 与 Character(0..2) 限制串；
// enumToString 的 << 与 ")" 各为独立语句（ORIG 重新 lea oss，非链式复用 %eax）。
#include "ServiceRestrictManager/serviceRestrictStringQuery.inc"
{
    std::ostringstream oss;
    for (int i = 0; i <= 0x21; ++i)
    {
        oss << "(" << 1 << "," << i << ",";
        oss << MobileRestrict::enumToString(static_cast<MobileRestrict::Enum>(i));
        oss << ")";
    }
    for (int i = 0; i <= 2; ++i)
    {
        oss << "(" << 2 << "," << i << ",";
        oss << CharacterRestrict::enumToString(static_cast<CharacterRestrict::Enum>(i));
        oss << ")";
    }
    return oss.str();
}
