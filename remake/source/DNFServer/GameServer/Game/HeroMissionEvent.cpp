// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 HeroMissionEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "HeroMissionEvent.h"

#include "CUser.h"
#include "LogManager.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "TaiwanInternal.h"      // Taiwan::internal_stream / TaiwanInternalPack::T
#include "TaiwanQueries.h"      // Taiwan::SigLoadHeroMissionData（init/GetInBuffer）
#include "GameTypes.h"          // cMyTrace（StartEvent/EndEvent trace）
#include "ClearDungeonMission.h"
#include "UseFatigueMission.h"
#include "UseCubeMission.h"
#include "LevelUpMission.h"
#include "AttendanceMission.h"
#include <boost/tokenizer.hpp> // char_separator / tokenizer（setEventData 切分）
#include "TotalMission.h"
#include <sstream>             // stringstream（getStringToken 拼串）
#include <string.h>            // strncpy（saveEventData 写回 SigSaveHeroMissionData）

#include "CItem.h"             // CItem::make_item（LevelUpMission 发奖虚调用 vptr+0x08）
#include "Inven_Item.h"        // Inven_Item / set_add_info
#include "CDataManager.h"      // G_CDataManager / find_item
#include "CMailBoxHelper.h"    // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "RDARScriptStringManager.h"  // g_scriptStringManager_ / findString
#include "DNFLexWrapper.h"     // loadRDARScriptFile / ScanType / ScanInt / ScanStr
#include "CEnvironment.h"      // less_TCHAR / SpinLock / G_CEnvironment
#include "CSystemTime.h"       // CSystemTime::getCurSec（AttendanceMission 判活时刻）

#include "HeroMissionEvent/importHeroMissionConditionsData.inc"
                                    const char* path);

// ---- 显式实例化：ARAD::Singleton<HeroMissionEvent>::Get()（ORIG W 0x0811c9ce）----
namespace ARAD
{
template HeroMissionEvent* Singleton<HeroMissionEvent>::Get();
}

#include "HeroMissionEvent/HeroMissionEvent.inc"
{
    // ORIG ctor 0x08166522：Singleton 基类 + CEventBase 基类（stADDINFO 置
    // 0xffff、flag 置 0）自动构造；m_conditions / m_values 默认构造。
}

// ============================================================================
// findMissionValue @ 0x08168010
//   bool findMissionValue(CUser*, MissionNo::T, HeroMissionValue*&)
//   判活 → mission ∈ [1,6] → m_values[charac_no] 的 vector 中匹配 mission。
// ============================================================================
#include "HeroMissionEvent/findMissionValue.inc"
                                        HeroMissionCondition::MissionNo::T mission,
                                        HeroMissionValue*& out)
{
    // ORIG 0x0816802e `call *%edx`：虚调用 CEventBase::IsEventing(NULL)（§4.5）。
    if (!IsEventing(0))
        return false;
    if (mission <= 0 || mission >= 7)
        return false;

    unsigned int characNo =
        static_cast<unsigned int>(user->get_charac_no(-1));
    std::map<unsigned int, std::vector<HeroMissionValue> >::iterator it =
        m_values.find(characNo);
    if (it == m_values.end())
        return false;

    for (std::vector<HeroMissionValue>::iterator vit = it->second.begin();
         vit != it->second.end(); ++vit)
    {
        // [b28d-solo-HeroMissionEvent] ORIG 0x081680e9 `mov (%eax),%eax;
        // cmp 0x10(%ebp),%eax`：去掉 `static_cast<int>(mission)` 后 4.4 直接
        // 内存源比较，省去 mov+mov 两段（N18b 的翻正形）。
        if (vit->m_missionType == mission)
        {
            out = &*vit;
            return true;
        }
    }
    return false;
}

// ============================================================================
// processMission @ 0x081672be
//   判活 → user 判空 → findMissionValue → m_conditions[mission] 条件对象
//   处理（虚表 +0x08）→ mission==4 且进度刚置位则递归触发 mission 6。
// ============================================================================
#include "HeroMissionEvent/processMission.inc"
                                      HeroMissionCondition::MissionNo::T mission,
                                      unsigned int param)
{
    // ORIG 0x081672dd `call *%edx`：虚调用 CEventBase::IsEventing(NULL)（§4.5）。
    if (!IsEventing(0))
        return;

    if (user == 0)
    {
        LogManager::logFormat(1, "localtaiwan/Event/EventHeroMission.cpp",
                              __PRETTY_FUNCTION__, 0x2b8,
                              "[HeroMission] User is null.");
        return;
    }

    HeroMissionValue* value = 0;
    // [b28d-solo-HeroMissionEvent] ORIG 0x081672d4 `call; test %al,%al; je tail`：
    // bool callee 的正向直判（2 条）；4.4 对 `!f()`/`f()==0` 恒补 xor $1（3 条）
    // ⇒ 改正向包裹（P222：否定早返 → 正向 if 包裹）。
    if (findMissionValue(user, mission, value))
    {
        std::map<HeroMissionCondition::MissionNo::T,
                 BaseHeroMissionCondition*>::iterator it =
            m_conditions.find(mission);
        if (it == m_conditions.end())
        {
            LogManager::logFormat(1, "localtaiwan/Event/EventHeroMission.cpp",
                                  __PRETTY_FUNCTION__, 0x2c2,
                                  "[HeroMission] Invalid mission. (charac:%u, mission:%d)",
                                  user->get_charac_no(-1),
                                  static_cast<int>(mission));
            user->DisConnSig(static_cast<DISCONN_SIG>(0x3e), 1, 0x2c3);
            return;
        }

        unsigned short prev = value->m_fieldA;
        it->second->processMission(user, *value, param);   // 条件虚表 +0x08
        // [b28d-solo-HeroMissionEvent] ORIG 0x08167470 `cmp -0xa(%ebp),%ax; jbe`
        // ⇒ 源码为 `> prev` 而非 `!= 0`。
        if (mission == 4 && prev == 0 && value->m_fieldA > prev)
        {
            processMission(user,
                           static_cast<HeroMissionCondition::MissionNo::T>(6), 1);
        }
    }
}

// ============================================================================
// ORIG T 0x0816709e sz 0x177（loadEventData.md 全文已读）：
// vptr+0x34 IsEventing(0) 为假直接返回；user 空 → log；否则
//   Acquire("localtaiwan/Event/EventHeroMission.cpp",0x291) → CStreamGuard(true)
//   → Taiwan::internal_stream(guard, TaiwanInternalPack::T 0x12, user->GetUID())
//   → guard->GetInBuffer<Taiwan::SigLoadHeroMissionData>()；空 → log；
//   否则 sig->init()；*(int*)sig = user->get_charac_no(-1)；
//   s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), guard)；栈 guard 析构。
// （ORIG try/catch 形态由 C++ 栈对象自动析构等价承载。）
// ============================================================================
#include "HeroMissionEvent/loadEventData.inc"
{
    // ORIG 0x081670be `call *%edx`：虚调用 CEventBase::IsEventing(NULL)（§4.5）。
    if (IsEventing(0) == 0)
        return;
    if (user == 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__, 0x28c,
            "[HeroMission] User is null.");
        return;
    }

    // [b28d-solo-HeroMissionEvent] ORIG 0x0816710c Acquire 结果直传 guard ctor
    // （无 stream 具名局部，P259）；具名局部多 mov 槽往返 2 条。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localtaiwan/Event/EventHeroMission.cpp", 0x291), true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(0x12),
                            static_cast<int>(user->GetUID()));
    Taiwan::SigLoadHeroMissionData* sig =
        guard.operator->()->GetInBuffer<Taiwan::SigLoadHeroMissionData>();
    if (sig == 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__, 0x296,
            "[HeroMission] SigLoadHeroMissionData is null.");
    }
    else
    {
        sig->init();
        // [b28d-solo-HeroMissionEvent] ORIG 0x081671c5 store 为 RHS-first
        // （`mov %eax,%edx; mov -0xc(%ebp),%eax; mov %edx,(%eax)`）⇒ P211 异型化：
        // 字段 unsigned（RHS int 异型）+ 具名成员访问（reinterpret_cast 视图会令
        // 4.4 把基址指针升格 %ebx 跨 call 保活，退化为 LHS-first）。
        sig->m_field0 = user->get_charac_no(-1);
        GlobalData::s_msgq_mgr->put(
            static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
    }
}

// ---------------------------------------------------------------------------
// setMissionValue —— ORIG T 0x08167216
// _ZN16HeroMissionEvent15setMissionValueEP5CUserN20HeroMissionCondition9MissionNo1TEj
// （LinkE2.JpnExImpl 批次：objdump 0x8167216..0x81672bc 逐指令转写）。
// 判活：ORIG vptr+0x34 = CEventBase::IsEventing(NULL)，实现即读 +0x08 标志
//（本头布局镜像注释同证）。
// ---------------------------------------------------------------------------
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
#include "HeroMissionEvent/setMissionValue.inc"
                                       HeroMissionCondition::MissionNo::T mission,
                                       unsigned int value)
{
    // ORIG 0x8167226：`mov (%eax);add $0x34;call *%edx` = 虚调用 IsEventing(NULL)。
    // 继承 CEventBase 后以真实虚调用复现（§4.5 正解），非直接读 m_isEventing。
    if (!IsEventing(0))
        return;

    if (user == 0)
    {
        LogManager::logFormat(1, "localtaiwan/Event/EventHeroMission.cpp",
                              __PRETTY_FUNCTION__,
                              0x2a6, "[HeroMission] User is null.");
        return;
    }

    HeroMissionValue* out = 0;   // ORIG -0xc(%ebp) = 0
    if (findMissionValue(user, mission, out))
    {
        // ORIG mov %dx,0x8(%eax)：HeroMissionValue +0x08 写 ushort value
        out->m_field8 = (unsigned short)value;
    }
    sendMissionData(user);
}
// ============================================================================
// ORIG 0x0816664a（StartEvent.md 全文已读）：loadScript() 失败（==0，P22
// 取反形态 xor $0x1;test;je）→ cMyTrace(__PRETTY_FUNCTION__,0x1fe,5)
// ("[HeroMission] Script load fail!!!")；成功 → SetEventFlag(true) + trace
// ("[HeroMission] Start!")。镜像布局以 m_isEventing=1 等价 SetEventFlag(true)。
// ============================================================================
#include "HeroMissionEvent/StartEvent.inc"
{
    // [b28c-L7] P221：ORIG 两互斥臂 cMyTrace 槽 fail=-0x28(深)/succ=-0x18(浅)，
    // 4.4 对**具名** tr 恒反（先声明浅，N93 推广族）。改「构造即调用」全表达式
    // 临时链 ⇒ 4.4 复现 ORIG 槽深（MySQL init_db_handle 同法 AE 实证）。
    if (!loadScript())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1fe, 5)
            ("[HeroMission] Script load fail!!!");
    }
    else
    {
        // ORIG 0x81666a8：loadScript 成功后 SetEventFlag(true)（写 +0x08 标志）
        SetEventFlag(true);
        cMyTrace(__PRETTY_FUNCTION__, 0x203, 0)("[HeroMission] Start!");
    }
}

// ============================================================================
// ORIG 0x081666e6（StartEvent_081666e6.md 全文已读）：thunk——经 vptr slot 2
// 虚调用 StartEvent()。镜像布局无 vtable，直接调用等价。
// ============================================================================
#include "HeroMissionEvent/StartEvent_1.inc"
{
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
    StartEvent();
}

// ============================================================================
// ORIG 0x08166700（EndEvent.md 全文已读）：先 cMyTrace("virtual void
// HeroMissionEvent::EndEvent()",0x20d,0)("[HeroMission] End!")，再
// SetEventFlag(false)。镜像布局以 m_isEventing=0 等价。
// ============================================================================
#include "HeroMissionEvent/EndEvent.inc"
{
    cMyTrace tr(__PRETTY_FUNCTION__, 0x20d, 0);
    tr("[HeroMission] End!");
    // ORIG 0x816674a：SetEventFlag(false)（写 +0x08 标志，与 IsEventing 同字段）
    SetEventFlag(false);
}

#include "HeroMissionEvent/TotalMission.inc"
    : BaseHeroMissionCondition(HeroMissionCondition::MissionNo::T_6)
{
}
#include "HeroMissionEvent/dtor_TotalMission.inc"
// ============================================================================
// TotalMission::processMission —— ORIG T 0x08166378（processMission.md 全文转写）。
// 合计任务推进：value.m_field4 按 param 累加，以 m_condition.m_extra.size()
// 为上限（区别于其它 mission 用 m_extra[0]）。
// ============================================================================
#include "HeroMissionEvent/processMission_1.inc"
                                  unsigned int param)
{
    // [b28d-solo-HeroMissionEvent] ORIG 0x0816637e 先 call size() 再读 field4，
    // 比较方向为 `size() <= (unsigned)field4`（setbe+jne 跳尾 return），
    // 条件 2 同向（setbe + je 跳 else 累加块）；原正向 `<` 写法产 setb+je 反极性。
    if (m_condition.m_extra.size() <= static_cast<unsigned int>(value.m_field4))
        return;
    if (m_condition.m_extra.size() <=
        static_cast<unsigned int>(value.m_field4) + param)
    {
        value.m_field4 = static_cast<unsigned short>(m_condition.m_extra.size());
    }
    else
    {
        value.m_field4 = value.m_field4 + static_cast<unsigned short>(param);
    }
}


#include "HeroMissionEvent/ClearDungeonMission.inc"
    : BaseHeroMissionCondition(HeroMissionCondition::MissionNo::T_1)
{
}
#include "HeroMissionEvent/dtor_ClearDungeonMission.inc"
// ============================================================================
// ClearDungeonMission::processMission —— ORIG T 0x081653b4（processMission.md）。
// 清图计数推进：m_extra[0] 为上限；missionType==2 且 field8==7 的任务不推进
// （特殊关卡类型屏蔽）。
// ============================================================================
#include "HeroMissionEvent/processMission_2.inc"
                                         unsigned int param)
{
    // [b28d-solo-HeroMissionEvent] ORIG 0x081653b4：条件 1 `limit <= field4`
    // （cmp %ax,%dx; setbe; jne 跳尾岛）；条件 2 短路结果整体 bool 物化
    // （mov $1/jmp/mov $0/test/jne）；条件 3 `limit <= field4+param` 钳制 fall-through。
    if (m_condition.m_extra[0] <= value.m_field4)
        return;
    if (getMissionType() == 2 && value.m_field8 != 7)
        return;
    if (m_condition.m_extra[0] <=
        static_cast<unsigned int>(value.m_field4) + param)
    {
        value.m_field4 = m_condition.m_extra[0];
    }
    else
    {
        value.m_field4 = value.m_field4 + static_cast<unsigned short>(param);
    }
}

// ============================================================================
// ClearDungeonMission::rewardMission —— ORIG T 0x08165484（rewardMission.md 全文）。
// 判活顺序：已奖励（非重复且 fieldA!=0）→ 任务条件不满足（type==2 且
// field8!=7）→ 计数未达上限 → rewardProcess 发奖并 fieldA++/field6=1，
// 重复任务 reset。
// ============================================================================
#include "HeroMissionEvent/rewardMission.inc"
{
    if (!isRepeatMission() && value.m_fieldA != 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0xac,
            "[HeroMission] Already reward. (charac:%u,mission:%d,count:%d)",
            user->get_charac_no(-1), value.m_missionType, value.m_fieldA);
        return false;
    }
    if (getMissionType() == 2 && value.m_field8 != 7)
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0xb4,
            "[HeroMission] Invalid reward condition. (charac:%u,mission:%d,value:%d)",
            user->get_charac_no(-1), value.m_missionType, value.m_field8);
        return false;
    }
    if (value.m_field4 < m_condition.m_extra[0])
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0xbb,
            "[HeroMission] Invalid reward condition. (charac:%u,mission:%d,count:%d)",
            user->get_charac_no(-1), value.m_missionType, value.m_fieldA);
        return false;
    }
    bool ok = rewardProcess(user);
    value.m_fieldA = value.m_fieldA + 1;
    value.m_field6 = 1;
    if (isRepeatMission())
    {
        value.reset();
    }
    return ok;
}

#include "HeroMissionEvent/UseFatigueMission.inc"
    : BaseHeroMissionCondition(HeroMissionCondition::MissionNo::T_2)
{
}
#include "HeroMissionEvent/dtor_UseFatigueMission.inc"
// ============================================================================
// UseFatigueMission::processMission —— ORIG T 0x08165706（processMission.md）。
// 疲劳消耗推进：m_extra[0] 为上限，field4+param 超限则钳制到上限。
// ============================================================================
#include "HeroMissionEvent/processMission_3.inc"
                                       unsigned int amount)
{
    // [b28d-solo-HeroMissionEvent] ORIG 0x08165706：先 call m_extra[0]（operator[]），
    // 比较方向 `limit <= field4`（cmp %ax,%dx; setbe; jne 跳尾 return）；条件 2 同向
    // （limit <= field4+param → fall-through 钳制，je 跳累加块）。
    if (m_condition.m_extra[0] <= value.m_field4)
        return;
    if (m_condition.m_extra[0] <=
        static_cast<unsigned int>(value.m_field4) + amount)
    {
        value.m_field4 = m_condition.m_extra[0];
    }
    else
    {
        value.m_field4 = value.m_field4 + static_cast<unsigned short>(amount);
    }
}

// ============================================================================
// UseFatigueMission::rewardMission —— ORIG T 0x081657a2（rewardMission.md）。
// 同 ClearDungeonMission::rewardMission 骨架（无 missionType 屏蔽分支）。
// ============================================================================
#include "HeroMissionEvent/rewardMission_1.inc"
{
    // [b28c-27] N（P168a X1 边界）：ORIG 首判 `call; xor $1,%eax; test %al; je` 与
    // 尾段 `call; test %al; je` 均来自 char 返回 callee；bool 化 isRepeatMission 会
    // 使 callee 自身 movzbl 直返劣化为 setne（ORIG 0x08164fc2 仅 movzbl+ret），
    // 调用点 X1（uchar ^=1）又被 4.4 因"判断后存在依赖同一条件的 bool 物化岛"
    // jump-thread 折叠（同文件 L462-466 已实证 11→13 回退）。保持 char + 短路原形。
    if (!isRepeatMission() && value.m_fieldA != 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0xe5,
            "[HeroMission] Already reward. (charac:%u,mission:%d,count:%d)",
            user->get_charac_no(-1), value.m_missionType, value.m_fieldA);
        return false;
    }
    if (value.m_field4 < m_condition.m_extra[0])
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0xec,
            "[HeroMission] Invalid reward condition. (charac:%u,mission:%d,count:%d)",
            user->get_charac_no(-1), value.m_missionType, value.m_fieldA);
        return false;
    }
    bool ok = rewardProcess(user);
    value.m_fieldA = value.m_fieldA + 1;
    value.m_field6 = 1;
    if (isRepeatMission())
    {
        value.reset();
    }
    return ok;
}

#include "HeroMissionEvent/UseCubeMission.inc"
    : BaseHeroMissionCondition(HeroMissionCondition::MissionNo::T_3)
{
}
#include "HeroMissionEvent/dtor_UseCubeMission.inc"
// ============================================================================
// UseCubeMission::processMission —— ORIG T 0x08165992（processMission.md）。
// 魔盒消耗推进：同 UseFatigueMission 骨架（m_extra[0] 上限钳制）。
// ============================================================================
#include "HeroMissionEvent/processMission_4.inc"
                                    unsigned int amount)
{
    // [b28d-solo-HeroMissionEvent] ORIG 0x08165992：与 UseFatigueMission 同骨架
    // （limit <= field4 跳尾；limit <= field4+param 钳制）。
    if (m_condition.m_extra[0] <= value.m_field4)
        return;
    if (m_condition.m_extra[0] <=
        static_cast<unsigned int>(value.m_field4) + amount)
    {
        value.m_field4 = m_condition.m_extra[0];
    }
    else
    {
        value.m_field4 = value.m_field4 + static_cast<unsigned short>(amount);
    }
}

// ============================================================================
// UseCubeMission::rewardMission —— ORIG T 0x08165a2e（rewardMission.md）。
// 同 UseFatigueMission::rewardMission 骨架（行号 0x117/0x11e）。
// [b28c-L26] 残余 4 行判词：
//  ① 首判 `call isRepeatMission; xor $1,%eax; test %al; je`——callee 为 char 返回
//     （ORIG 0x08164fc2 仅 movzbl+ret），bool 化会毁 callee 本体（P168a③b/N 跨版本
//     bool ABI）；X1 `uchar ^=1` 形又会被 4.4 jump-thread 折叠掉紧随的
//     `value.m_fieldA!=0` bool 物化岛（实测 11→13 回退）⇒ 保持短路裸形，缺 1 xor。
//  ② 尾段 `call; test %al,%al; je`（char callee 直判、无 setne）——4.4 对 char 返回
//     在 if() 里恒补 setne 归一，实测本形 0 条差改裸形反增；维持。
//  ③ `mov (%eax),%eax; mov %eax,%ebx` 两段：ORIG 4.1 冗余 regmove（N18a 反向，
//     we-smaller 1 条），4.4 无源形可造。
//  ④ 其余 diff 全为 call/jmp 目标与字符串/行号地址的 .o 重定位占位（§6.3）。
// ============================================================================
#include "HeroMissionEvent/rewardMission_2.inc"
{
    // X1 形在此处**不适用**（实测 ndiff 11→13 变差，已回退）：
    // ORIG @0x08165a2e 把 `value.m_fieldA != 0` 物化成 bool 值
    // （`test %ax,%ax; je L; mov $1,%eax; jmp end; L: mov $0,%eax; end:
    //   test %al,%al; je BODY`），而一旦把前面的 `!isRepeatMission()` 改写成
    //   `register uchar + ^=1 + goto`，4.4 的 jump threading 就会顺手把这段
    //   物化岛折叠掉（缺 5 条）。保持 `&&` 短路原写法反而更接近 ORIG。
    // ⇒ X1 适用前提：**判断之后没有依赖同一条件的 bool 物化岛**。
    // [b28c-L26 档①+② 后追加 P84] bool 化 isRepeatMission 使 xor 段归位；剩余
    // 缺项是 ORIG 把整个 `!isRepeat() && fieldA!=0` 短路结果物化成 bool 局部
    // （`mov $1,%eax; jmp; mov $0,%eax; test %al,%al; je BODY`）⇒ 按 P84 具名 bool。
    // [b28d-solo-HeroMissionEvent] P226 扩展：ORIG 0x08165a2e 把短路结果在 al 中
    // 物化后 `test %al,%al; je`（零槽）；具名 bool 会落栈槽（mov %al,-0xa;
    // cmpb $0,-0xa）。语句表达式（内不含具名变量）复现零槽物化岛。
    if (({ !isRepeatMission() && value.m_fieldA != 0; }))
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0x117,
            "[HeroMission] Already reward. (charac:%u,mission:%d,count:%d)",
            user->get_charac_no(-1), value.m_missionType, value.m_fieldA);
        return false;
    }
    if (value.m_field4 < m_condition.m_extra[0])
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0x11e,
            "[HeroMission] Invalid reward condition. (charac:%u,mission:%d,count:%d)",
            user->get_charac_no(-1), value.m_missionType, value.m_fieldA);
        return false;
    }
    bool ok = rewardProcess(user);
    value.m_fieldA = value.m_fieldA + 1;
    value.m_field6 = 1;
    if (isRepeatMission())
    {
        value.reset();
    }
    return ok;
}

#include "HeroMissionEvent/LevelUpMission.inc"
    : BaseHeroMissionCondition(HeroMissionCondition::MissionNo::T_4)
{
}
#include "HeroMissionEvent/dtor_LevelUpMission.inc"
// ============================================================================
// LevelUpMission::processMission —— ORIG T 0x08165c1e（processMission.md 全文）。
// 升级任务：m_extra[0] 为等级上限，field8 记录上次等级；达到新等级时遍历
// m_rewards 逐项构造 Inven_Item 并以系统邮件发放，随后 fieldA++/field6=1、
// field8 记当前等级，重复任务 reset。
// [L3EHeroItemUpgrade 2026-09-11 复核（未改）] 本件已达 ndiff 10 / ratio 0.961
//   （8 件中最接近 identical 者）：残余仅 ① 0x08165d44 的 `mov (%eax),%eax;
//   mov %eax,%ebx` 两段实参物化（4.4 折单段 `mov (%eax),%ebx`，N18a；P254/P265
//   已实测 enum 化与 cast 形均无效）② ORIG 尾 `nop` 对齐岛。**判 N、零改动**。
// ============================================================================
#include "HeroMissionEvent/processMission_5.inc"
                                    unsigned int /*param*/)
{
    if (m_condition.m_extra[0] > user->get_charac_level())
        return;
    if (!isRepeatMission() && value.m_fieldA != 0)
        return;
    if (value.m_field8 >= user->get_charac_level())
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0x145,
            "[HeroMission] Invalid reward condition. (charac:%u,mission:%d,value:%d)",
            user->get_charac_no(-1), value.m_missionType, value.m_field8);
        return;
    }

    std::vector<HeroMissionCondition::RewardData>::iterator it =
        m_condition.m_rewards.begin();
    std::vector<HeroMissionCondition::RewardData>::iterator end =
        m_condition.m_rewards.end();
    for (; it != end; ++it)
    {
        // [b28d-solo-HeroMissionEvent] N18a 封顶：ORIG 0x08165d3b 先物化实参
        // `it->m_itemIdx`（mov (%eax),%eax; mov %eax,%ebx）再 call G_CDataManager；
        // 4.4 恒 receiver 先且折单段 mov (%eax),%ebx。P254 enum 化实测净零——
        // 字段枚举化（HeroMissionCondition.h 临时实验，已回退）使本处两段翻正，
        // 但同字段日志路径 0x08165e3a（ORIG `mov (%eax),%edi` 单段）被拖成两段，
        // gap 1→-1；P265 实参 cast 形亦无效。同字段多使用点方向相反 ⇒ 源形不可
        // 同时满足，判 N18a。
        CItem* item = G_CDataManager()->find_item(it->m_itemIdx);
        if (item == 0)
        {
            // [b28d-solo-HeroMissionEvent] ORIG 0x08165e3a 先求值 varargs
            // （edi/esi/ebx）再 call cMyTrace::C1（P227 全表达式临时链）。
            cMyTrace(__PRETTY_FUNCTION__, 0x150, 5)
                ("[HeroMission] Invalid item. (charac:%u, mission:%d, item:%u)",
                 user->get_charac_no(-1), m_missionCode, it->m_itemIdx);
            return;
        }
        Inven_Item mailItem;
        // [b28d-solo-HeroMissionEvent] P171 死写入照抄：ORIG 0x08165dbd
        // `mov (%eax),%eax; mov %eax,-0x77(%ebp)`（mailItem.m_addInfo=it->m_itemIdx，
        // 随后被 set_add_info 覆盖）。
        mailItem.m_addInfo = it->m_itemIdx;
        item->make_item(mailItem);
        mailItem.set_add_info(it->m_addInfo);
        const char* title = g_scriptStringManager_.findString(
            4, m_condition.m_mailTitle.c_str(), 0);
        const char* content = g_scriptStringManager_.findString(
            4, m_condition.m_mailContent.c_str(), 0);
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            title, mailItem, 0, user->get_charac_no(-1), content,
            strlen(content), 0, user->GetServerGroup(), false, false);
    }

    value.m_fieldA = value.m_fieldA + 1;
    value.m_field6 = 1;
    value.m_field8 = static_cast<unsigned short>(user->get_charac_level());
    if (isRepeatMission())
    {
        value.reset();
    }
}

// ============================================================================
// LevelUpMission::rewardMission —— ORIG T 0x08165f86（rewardMission.md）：
// 恒返回 true（升级奖励在 processMission 路径发放，无独立 reward 流程）。
// ============================================================================
#include "HeroMissionEvent/rewardMission_3.inc"
{
    return true;
}

#include "HeroMissionEvent/AttendanceMission.inc"
    : BaseHeroMissionCondition(HeroMissionCondition::MissionNo::T_5)
{
}
#include "HeroMissionEvent/dtor_AttendanceMission.inc"
// ============================================================================
// AttendanceMission::processMission —— ORIG T 0x08166002（processMission.md）。
// 签到：以 fatigueTime 为每日重置时刻；m_extra[0] 为签到上限、[1] 为开始
// 月份、[2] 为开始日；与目标日相差超过 1 天（0x15180 秒）先 reset，再
// field4+1（钳制到上限）并 field8 累加相差天数。
// ============================================================================
#include "HeroMissionEvent/processMission_6.inc"
                                       unsigned int /*param*/)
{
    // [b28d-solo-HeroMissionEvent] ORIG 0x08166002 条件 1 为 `limit <= field4`
    // （cmp %ax,%dx; setbe; jne 跳尾 return）；tm 槽序 today=-0x40 浅于
    // target=-0x6c ⇒ 4.4「先声明浅」需 today 先声明。
    if (m_condition.m_extra[0] <= value.m_field4)
        return;
    {
        struct tm today;       // ORIG -0x40
        struct tm target;      // ORIG -0x6c
        time_t cur;            // ORIG -0x14
        time_t todaySec;       // ORIG -0x10
        time_t targetSec;      // ORIG -0xc
        cur = GlobalData::s_systemTime_.getCurSec();
        localtime_r(&cur, &today);
        localtime_r(&cur, &target);
        today.tm_hour = G_CEnvironment()->m_serverEnvir.m_fatigueTime;
        today.tm_min = 0;
        today.tm_sec = 0;
        todaySec = mktime(&today);
        target.tm_mon = m_condition.m_extra[1] - 1;
        target.tm_mday = m_condition.m_extra[2] + value.m_field8;
        target.tm_hour = G_CEnvironment()->m_serverEnvir.m_fatigueTime;
        target.tm_min = 0;
        target.tm_sec = 0;
        targetSec = mktime(&target);
        if (0x15180 < todaySec - targetSec)
        {
            value.reset();
        }
        // [b28d-solo-HeroMissionEvent] ORIG 0x0816613e 条件 3 为
        // `limit <= field4+1`（cmp %eax,%edx; setle; je 跳 else 累加块）。
        if (m_condition.m_extra[0] <= value.m_field4 + 1)
        {
            value.m_field4 = m_condition.m_extra[0];
        }
        else
        {
            value.m_field4 = value.m_field4 + 1;
        }
        value.m_field8 = value.m_field8 +
            static_cast<unsigned short>((todaySec - targetSec) / 0x15180);
    }
}

// ============================================================================
// AttendanceMission::rewardMission —— ORIG T 0x081661d6（rewardMission.md）。
// 骨架同 UseFatigueMission::rewardMission；已奖励分支无日志，条件不满足分支
// 用 cMyTrace（0x1a4, lvl 5）而非 logFormat。
// ============================================================================
#include "HeroMissionEvent/rewardMission_4.inc"
{
    if (!isRepeatMission() && value.m_fieldA != 0)
    {
        return false;
    }
    if (value.m_field4 < m_condition.m_extra[0])
    {
        // [b28d-solo-HeroMissionEvent] ORIG 0x081661d6 先求值 varargs（edi/esi/ebx）
        // 再 call cMyTrace::C1，最后 call operator()（P227 全表达式临时链）。
        cMyTrace(__PRETTY_FUNCTION__, 0x1a4, 5)
            ("[HeroMission] Reward condition error. (charac:%u,mission:%d,count:%d)",
             user->get_charac_no(-1), value.m_missionType, value.m_fieldA);
        return false;
    }
    bool ok = rewardProcess(user);
    value.m_fieldA = value.m_fieldA + 1;
    value.m_field6 = 1;
    if (isRepeatMission())
    {
        value.reset();
    }
    return ok;
}

// ============================================================================
// ConvertString2EnumMissionType —— ORIG t 0x89613d7（无报告，objdump 0x 转写）。
// [type] 标签的字符串枚举 → 整型（"normal"→1 / "payment"→2 / "final"→3）。
// 函数级 static 惰性表（lookup @0x94f7598 / SpinLock @0x94f75b0 / 就绪字节
// @0x94f75b4，双检锁 + 首轮 fallthrough 建表后重查，P21 范式）。
// [b28d-solo-HeroMissionEvent] 判词 AE：fresh clean ndiff=0（orig=213 our=213，
// gap=0）；清单 NEAR19 系旧读数。
// ============================================================================
static int ConvertString2EnumMissionType(std::string& s)
{
loop_top:
    static std::map<const char*, int, less_TCHAR> __lookup;
    static SpinLock __init_lock;
    static bool __inited;
    int code;
    if (__inited)
    {
        std::map<const char*, int, less_TCHAR>::const_iterator it =
            __lookup.find(s.c_str());
        code = (it != __lookup.end()) ? it->second : -1;
    }
    else
    {
        __init_lock.enter();
        if (__inited)
        {
            __init_lock.leave();
            goto loop_top;
        }
        code = 0;
    }

    switch (code)
    {
    case 0:
    case 0x3d1:   // "normal"
        if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("normal", 0x3d1));
        else
            return 1;
    case 0x3d3:   // "payment"
        if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("payment", 0x3d3));
        else
            return 2;
    case 0x3d5:   // "final"
        if (!__inited)
            __lookup.insert(std::pair<const char* const, int>("final", 0x3d5));
        else
            return 3;
    }

    if (!__inited)
    {
        __init_lock.leave();
        __inited = true;
        goto loop_top;
    }
    return 0;
}

// ============================================================================
// importHeroMissionConditionsData —— ORIG T 0x896171b（无报告，nm+objdump 0x
// 逐指令转写）。RDA 脚本 "Event/HeroMissionEvent.evt" 解析：
//   [/hero event] [mission] [type] [repeat] [reset] [item] [condition]
//   [mail title] [mail content] [/mission]
// 返回：0=成功；非 0=出错脚本行号（loadScript 以 0 判活）。
// 首轮经 switch fallthrough 链惰性填充标签表（__lookup @0x94f7578 / SpinLock
// @0x94f7590 / 就绪字节 @0x94f7594，P21 范式），随后重查当前行分派。
// ============================================================================
// importHeroMissionConditionsData @ 0x0896171b —— [b28d-solo-HeroMissionEvent ·
//   N89/N82 封顶] 732 vs 717、gap 15、p167 CLEAR（0 missing/0 extra）。
//   剩余 ORIG-only 18 条 = movl 6（dead 局部初始化）+ jmp 8（决策树块排布）
//   + xor/jg/movw/add 各 1；槽基差 4（ORIG cond -0x16c ↔ OURS -0x170）属 N95；
//   4.1 对 vector/map 模板 out-of-line 实例（N89）+ 4.4 槽分配序（N82）不可翻。
//   配套：RewardData 用户空析构删除（P317）后 EXTRA +2 D1 消失。
#include "HeroMissionEvent/importHeroMissionConditionsData_1.inc"
                                    const char* path)
{
    if (!loadRDARScriptFile("", path))
        return 0x3e0;
    if (out == 0)
        return 0x3e3;

    std::string line;
    HeroMissionCondition cond;
    cond.clear();

    while (ScanType(line, true))
    {
    loop_top:
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited;

        int code;
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(line.c_str());
            code = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                continue;
            }
            code = 0;
        }

        switch (code)
        {
        case 0:
        case 0x3f0:   // [/hero event]
            if (!__inited)
                __lookup.insert(
                    std::pair<const char* const, int>("[/hero event]", 0x3f0));
            else
                return 0;
        case 0x3f4:   // [mission]
            if (!__inited)
                __lookup.insert(
                    std::pair<const char* const, int>("[mission]", 0x3f4));
            else
            {
                int t;
                if (!ScanInt(&t))
                    return 0x3f7;
                if (t <= 0 || t > 6)
                    return 0x3fc;
                cond.m_missionType = t;
            }
        case 0x400:   // [type]
            if (!__inited)
                __lookup.insert(
                    std::pair<const char* const, int>("[type]", 0x400));
            else
            {
                std::string s;
                if (!ScanStr(&s))
                    return 0x403;
                int t = ConvertString2EnumMissionType(s);
                if (t == 0)
                    return 0x407;
                cond.m_field04 = t;
            }
        case 0x40c:   // [repeat]
            if (!__inited)
                __lookup.insert(
                    std::pair<const char* const, int>("[repeat]", 0x40c));
            else
            {
                std::string s;
                if (!ScanStr(&s))
                    return 0x40f;
                cond.m_isRepeatMission = (s == "true");
            }
        case 0x414:   // [reset]
            if (!__inited)
                __lookup.insert(
                    std::pair<const char* const, int>("[reset]", 0x414));
            else
            {
                std::string s;
                if (!ScanStr(&s))
                    return 0x417;
                cond.m_isResetState = (s == "true");
            }
        case 0x41c:   // [item]
            if (!__inited)
                __lookup.insert(
                    std::pair<const char* const, int>("[item]", 0x41c));
            else
            {
                int cnt;
                if (!ScanInt(&cnt))
                    return 0x421;
                for (int i = 0; i < cnt; ++i)
                {
                    HeroMissionCondition::RewardData reward;
                    if (!ScanInt(&reward.m_itemIdx))
                        return 0x428;
                    int add;
                    if (!ScanInt(&add))
                        return 0x42b;
                    reward.m_addInfo = static_cast<unsigned short>(add);
                    cond.m_rewards.push_back(reward);
                }
            }
        case 0x431:   // [condition]
            if (!__inited)
                __lookup.insert(
                    std::pair<const char* const, int>("[condition]", 0x431));
            else
            {
                int cnt;
                if (!ScanInt(&cnt))
                    return 0x436;
                for (int i = 0; i < cnt; ++i)
                {
                    int v;
                    if (!ScanInt(&v))
                        return 0x43c;
                    cond.m_extra.push_back(static_cast<unsigned short>(v));
                }
            }
        case 0x442:   // [mail title]
            if (!__inited)
                __lookup.insert(
                    std::pair<const char* const, int>("[mail title]", 0x442));
            else
            {
                std::string s;
                if (!ScanStr(&s))
                    return 0x445;
                cond.m_mailTitle = s;
            }
        case 0x44a:   // [mail content]
            if (!__inited)
                __lookup.insert(
                    std::pair<const char* const, int>("[mail content]", 0x44a));
            else
            {
                std::string s;
                if (!ScanStr(&s))
                    return 0x44d;
                cond.m_mailContent = s;
            }
        case 0x452:   // [/mission]
            if (!__inited)
                __lookup.insert(
                    std::pair<const char* const, int>("[/mission]", 0x452));
            else
            {
                for (std::vector<HeroMissionCondition>::iterator it = out->begin();
                     it != out->end(); ++it)
                {
                    if (it->m_missionType == cond.m_missionType)
                        return 0x456;
                }
                out->push_back(cond);
                cond.clear();
            }
        }

        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto loop_top;
        }
    }
    return 0;
}

// loadScript @ 0x08167ac8 —— [b28d-solo-HeroMissionEvent · N104 封顶]
//   insn 364=364、帧 0x6c 相同、p167 仅缺 1 次 _Unwind_Resume：4.4 把
//   new-cleanup 汇入 catch 尾块，4.1 走 _Unwind_Resume（N104 EH 尾块相对序）。
//   语义完成（new×6 + import + 条件遍历 + logFormat 调用点全数对齐）。
// [L3EHeroItemUpgrade 2026-09-11 负结论] ORIG 0x08167ad1 与 0x08167e2c 有两条
//   稀疏 `movl $0x0,slot`（P331 声明序：`p = 0` 在 try 边界前、`line = 0` 在
//   vector ctor 前）。实测改写为两语句形（`BaseHeroMissionCondition* p = 0;
//   p = new ...` / `int line = 0; ... line = import(...)`）后：那两条**可复现**，
//   但 our_insns 364 → **366**（gap 0 → -2，4.4 在 EH 尾块别处多 2 条抵消），
//   editdist 117 不变、sequence ratio 0.7874 → 0.7852 ⇒ 无净收益，回滚。
#include "HeroMissionEvent/loadScript.inc"
{
	try {
    BaseHeroMissionCondition* p = new ClearDungeonMission();
    if (p == 0)
        throw 0x345;
    m_conditions[static_cast<HeroMissionCondition::MissionNo::T>(p->getMissionCode())] = p;

    p = new UseFatigueMission();
    if (p == 0)
        throw 0x349;
    m_conditions[static_cast<HeroMissionCondition::MissionNo::T>(p->getMissionCode())] = p;

    p = new UseCubeMission();
    if (p == 0)
        throw 0x34d;
    m_conditions[static_cast<HeroMissionCondition::MissionNo::T>(p->getMissionCode())] = p;

    p = new LevelUpMission();
    if (p == 0)
        throw 0x351;
    m_conditions[static_cast<HeroMissionCondition::MissionNo::T>(p->getMissionCode())] = p;

    p = new AttendanceMission();
    if (p == 0)
        throw 0x355;
    m_conditions[static_cast<HeroMissionCondition::MissionNo::T>(p->getMissionCode())] = p;

    p = new TotalMission();
    if (p == 0)
        throw 0x359;
    m_conditions[static_cast<HeroMissionCondition::MissionNo::T>(p->getMissionCode())] = p;

    std::vector<HeroMissionCondition> conds;
    // [C07 · Main 新轴 A(cv 限定符) 试形已回退] `const int line` → `int line` 实测
    //   本件四数与 loadScript ndiff **零变化**（364/364、ndiff 638 不变）⇒ 无收益回退。
    const int line = importHeroMissionConditionsData(&conds, "Event/HeroMissionEvent.evt");
    if (line == 0)
    {
        for (std::vector<HeroMissionCondition>::iterator it = conds.begin();
             it != conds.end(); ++it)
        {
            if (it->m_missionType >= 1 && it->m_missionType <= 6)
            {
                HeroMissionCondition::MissionNo::T code =
                    static_cast<HeroMissionCondition::MissionNo::T>(it->m_missionType);
                // [b28d-solo-HeroMissionEvent] ORIG 0x08167f7d 直接
                // `m_conditions[code]->setConditionData(*it)`（operator[]，无
                // find/end/判空；P167 差集 MISSING operator[] / EXTRA find+end+ne）。
                m_conditions[code]->setConditionData(*it);
            }
        }
        conds.clear();
        return true;
    }
    LogManager::logFormat(
        1, "localtaiwan/Event/EventHeroMission.cpp",
        __PRETTY_FUNCTION__, 0x367,
        "[HeroMission] Load script fail. (line:%d)", line);
    return false;
	} catch (int e) {
		// ORIG 0x08167e60-0x08167eb2：new 失败 throw 被本函数 catch 消化，
		// logFormat(0x35e, "[HeroMission] Fail create mission. (line:%d)", e)
		// 后返回 false，异常不向上传播（NEW 原实现裸 throw 会逃逸）。
		LogManager::logFormat(
			1, "localtaiwan/Event/EventHeroMission.cpp",
			__PRETTY_FUNCTION__, 0x35e,
			"[HeroMission] Fail create mission. (line:%d)", e);
		return false;
	}
}
// ============================================================================
// setEventData @ 0x08166752（setEventData.md 全文转写；定义归本 TU，
// 与 HeroMissionLink4.cpp 旧定义同符号，由 Main 整合去重）。
// 判活 → charac_no 非 0 → 空数据按条件表生成初始单（level==70 时任务 4/6
// 置完成态）；非空按 "," 切分逐 token setStringToken；最后
// find 已存在 → log(0x24b)+erase，再 insert(make_pair(charac_no, values))。
// ============================================================================
#include "HeroMissionEvent/setEventData.inc"
{
    // ORIG 0x08166775 `call *%edx`：虚调用 CEventBase::IsEventing(NULL)（§4.5）。
    if (IsEventing(0) == 0)
    {
        return;
    }
    if (user->get_charac_no(-1) == 0)
    {
        return;
    }

    HeroMissionValue value;
    std::vector<HeroMissionValue> values;
    std::string scriptData(data);

    if (scriptData.length() == 0)
    {
        // 空数据：按条件表生成初始值单（满级 70 时任务 4/6 直接置完成态）
        for (std::map<HeroMissionCondition::MissionNo::T,
                      BaseHeroMissionCondition*>::iterator cit =
                 m_conditions.begin();
             cit != m_conditions.end(); ++cit)
        {
            value.clear();
            value.m_mission = cit->first;
            if (cit->first == HeroMissionCondition::MissionNo::T_4 &&
                user->get_charac_level() == 70)
            {
                value.m_field4 = 1;
                value.m_field6 = 1;
                value.m_field8 = 70;
                value.m_fieldA = 1;
            }
            if (cit->first == HeroMissionCondition::MissionNo::T_6 &&
                user->get_charac_level() == 70)
            {
                value.m_field4 = 1;
            }
            values.push_back(value);
        }
    }
    else
    {
        // ORIG: boost::char_separator(",") 切分，逐 token setStringToken
        boost::char_separator<char> separator(",");
        boost::tokenizer<boost::char_separator<char> > tokens(scriptData,
                                                              separator);
        for (boost::tokenizer<boost::char_separator<char> >::iterator tit =
                 tokens.begin();
             tit != tokens.end(); ++tit)
        {
            // [b28d-solo-HeroMissionEvent] ORIG 0x08166a5c 走 boost
            // iterator_facade::operator->() + std::string::C1(char const*,
            // allocator const&) ⇒ 源码传 c_str()（隐式构造 string），非 *tit。
            value.setStringToken(tit->c_str());
            values.push_back(value);
        }
    }

    // [b28d-solo-HeroMissionEvent] ORIG 0x08166b37/0x08166ba0/0x08166be3/
    // 0x08166c0e 各独立调用 user->get_charac_no(-1)（无 characNo 具名局部，
    // 临时槽 -0x40/-0x38/-0xc；make_pair 的 T=int）。
    std::map<unsigned int, std::vector<HeroMissionValue> >::iterator it =
        m_values.find(user->get_charac_no(-1));
    if (it != m_values.end())
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__, 0x24b,
            "[HeroMission] Already exist character data. (charac:%u)",
            user->get_charac_no(-1));
        m_values.erase(user->get_charac_no(-1));
    }
    m_values.insert(std::make_pair(user->get_charac_no(-1), values));
}
// ============================================================================
// HeroMissionValue::getStringToken —— ORIG T 0x08164bca（getStringToken.md
// 全文转写；saveEventData 依赖，定义归本 TU）。
// 若 out 非空先补 ','，再按 "mission f4 f6 fA f8" 空格分隔追加（字段序与
// setStringToken 的解析偏移一一对应：tok0→m_mission、tok1→+0x04、tok2→+0x06、
// tok3→+0x0a、tok4→+0x08）。
// [b28d-solo-HeroMissionEvent] 判词 AE：fresh clean ndiff=0（orig=111 our=111，
// gap=0）；清单 NEAR1 系旧读数。修法=枚举视图选 `_ZNSolsEi` + 链拆两语句。
// ============================================================================
#include "HeroMissionEvent/getStringToken.inc"
{
    std::stringstream ss;
    if (out.length() != 0)
    {
        ss << ',';
    }
    // [b28d-solo-HeroMissionEvent] ORIG 0x08164c47 输出 m_mission 走
    // `_ZNSolsEi`（int 重载）⇒ 用枚举视图（枚举提升 int），unsigned 视图会选
    // `_ZNSolsEj` 造成符号级错配。
    ss << m_missionType << ' ' << m_field4 << ' ';
    ss << m_field6 << ' ' << m_fieldA << ' ' << m_field8;
    out += ss.str();
}
// ============================================================================
// setEventData(std::string const&) —— ORIG T 0x08166d0e（setEventData_08166d0e.md
// 全文转写）：thunk——c_str() 后转调 char const* 版本。
// ============================================================================
#include "HeroMissionEvent/setEventData_1.inc"
{
    setEventData(user, data.c_str());
}
// ============================================================================
// setMissionForGm —— ORIG T 0x081679ac（setMissionForGm.md 全文转写）。
// findMissionValue 命中且 m_conditions 中存在该 mission 时，
// 写 HeroMissionValue +0x04（m_field4）为 ushort 截断的 GM 设定值。
// ============================================================================
#include "HeroMissionEvent/setMissionForGm.inc"
                                       HeroMissionCondition::MissionNo::T mission,
                                       unsigned int value)
{
    HeroMissionValue* out = 0;   // ORIG -0x10(%ebp) = 0
    if (findMissionValue(user, mission, out))
    {
        std::map<HeroMissionCondition::MissionNo::T,
                 BaseHeroMissionCondition*>::iterator it =
            m_conditions.find(mission);
        // [b28c-L7] P223：ORIG 0x08167a28 `test %al; jne <nop>` + body 尾
        // `jmp <leave>` + `+0x8a: nop` 空块占位岛 ⇒ 复现形 `if(it==end) goto END;
        // body; END:;`（4.4 对 `if(it!=end){body}` 恒 jump-thread 少 jmp/nop）。
        if (it == m_conditions.end())
            goto END;
        // ORIG mov %dx,0x4(%eax)：HeroMissionValue +0x04 写 ushort GM 值
        out->m_field4 = (unsigned short)value;
    }
END:;
}
// ============================================================================
// resetMissionForGm —— ORIG T 0x08167a3a（resetMissionForGm.md 全文转写）。
// findMissionValue 命中且 m_conditions 中存在该 mission 时，调
// HeroMissionValue::reset()（+0x04..+0x0a 清零）。
// ============================================================================
#include "HeroMissionEvent/resetMissionForGm.inc"
                                         HeroMissionCondition::MissionNo::T mission)
{
    HeroMissionValue* out = 0;   // ORIG -0x10(%ebp) = 0
    if (findMissionValue(user, mission, out))
    {
        std::map<HeroMissionCondition::MissionNo::T,
                 BaseHeroMissionCondition*>::iterator it =
            m_conditions.find(mission);
        // [b28c-L7] P223：ORIG 0x08167aXX 同 setMissionForGm 空块 nop 岛形。
        if (it == m_conditions.end())
            goto END;
        out->reset();
    }
END:;
}
// ============================================================================
// saveEventData —— ORIG T 0x08166d38（saveEventData.md 全文转写）。
// 判活 → user 判空 → charac_no 非 0 → m_values.find(charac_no) 不存在则 log
// 0x269 返回；存在则逐 HeroMissionValue getStringToken 拼 "," 分隔串；
// Acquire(0x271) → CStreamGuard(true) → Taiwan::internal_stream(guard, T 0x11,
// user->GetUID()) → GetInBuffer<SigSaveHeroMissionData>()；空 → log 0x276；
// 否则 sig->init()；*(int*)sig = charac_no；strncpy(sig+0x04, data, len)；
// s_msgq_mgr->put(QUEUE_IDX(2), guard)；removeData 为真再 erase(charac_no)。
// （ORIG try/catch 形态由栈对象自动析构等价承载。）
// ============================================================================
// saveEventData @ 0x08166d38 —— [b28d-solo-HeroMissionEvent · N95 封顶]
//   insn 225=225、帧 0x60 相同、p167 CLEAR；全部局部槽整体深 4
//   （ORIG -0xc/-0x20/-0x24/-0x2c/-0x30 ↔ OURS -0x10/-0x24/-0x28/-0x30/-0x34），
//   属 4.4 局部槽基/分配序跨版本（N95）。P259 删 stream 局部实测 gap 0→2 反劣
//   （已回退）；data/characNo 两声明序均试不闭合。
// [L3EHeroItemUpgrade 2026-09-11] 复核前述 P259 结论并**翻案**（判据改按 ndiff）：
//   ① 0x08166f0d..0x08166f24：StreamPool::Acquire 返回值 `%eax` **直传**
//      CStreamGuard ctor 第 2 实参（`movl $0x1,0x8(%esp); mov %eax,0x4(%esp)`）
//      ⇒ 删 `Stream* stream` 具名局部；
//   ② 0x08166fda..0x08166fe8：strncpy 实参处才 `mov -0xc(%ebp),%edx; add $0x4,
//      %edx`（-0xc 即 sig 本身）⇒ 删 `char* dst` 具名局部。
//   实测：our 227 → **223**、**ndiff（= 工具 n_diff 编辑距离口径）57 → 19**、
//   ratio .771 → .929，TU 四数不变 ⇒ 按「gap 或 ndiff 严格改善」判据**保留**
//   （gap 由 0 变 +4 属 insn 计数差，非结构劣化：残余仅 logFormat 串在 .o 中为
//   .rodata 重定位（工具渲染差）、0x08166f93 `mov $0x0,%ebx` 与 0x08166ffe
//   `mov %eax,%edx` 系 4.1 返回/寄存器约定（N7）、尾对齐 nop）。
#include "HeroMissionEvent/saveEventData.inc"
{
    // ORIG 0x08166d5e `call *%edx`：虚调用 CEventBase::IsEventing(NULL)（§4.5）。
    if (IsEventing(0) == 0)
    {
        return;
    }
    if (user == 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__, 0x25e,
            "[HeroMission] User is null.");
        return;
    }
    if (user->get_charac_no(-1) == 0)
    {
        return;
    }

    // [b28d-solo-HeroMissionEvent] ORIG 0x08166d8e 先构造 `std::string data`
    // （-0x20 槽，call _ZNSsC1Ev）再 call get_charac_no；槽序 characNo(-0x1c)
    // 浅于 data(-0x20) ⇒ 4.4「先声明浅」要求 characNo 先声明、赋值分两句。
    unsigned int characNo;
    std::string data;
    characNo = (unsigned int)user->get_charac_no(-1);
    std::map<unsigned int, std::vector<HeroMissionValue> >::iterator it =
        m_values.find(characNo);
    if (it == m_values.end())
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__, 0x269,
            "[HeroMission] Not exist mission value. (charac:%u)",
            user->get_charac_no(-1));
        return;
    }

    for (std::vector<HeroMissionValue>::iterator vit = it->second.begin();
         vit != it->second.end(); ++vit)
    {
        vit->getStringToken(data);
    }

    // [L3EHeroItemUpgrade 2026-09-11] ORIG 0x08166f0d..0x08166f24：Acquire 返回值
    // `%eax` 直传 CStreamGuard ctor 的第 2 实参（`movl $0x1,0x8(%esp);
    // mov %eax,0x4(%esp)`），**无具名 stream 槽** ⇒ 删具名局部（P183/P48）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire(
            "localtaiwan/Event/EventHeroMission.cpp", 0x271),
        true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(0x11),
                            static_cast<int>(user->GetUID()));
    Taiwan::SigSaveHeroMissionData* sig =
        guard.operator->()->GetInBuffer<Taiwan::SigSaveHeroMissionData>();
    if (sig == 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__, 0x276,
            "[HeroMission] SigSaveHeroMissionData is null.");
    }
    else
    {
        sig->init();
        // [b28d-solo-HeroMissionEvent] ORIG 0x0816701e/0x08167037 各再调一次
        // user->get_charac_no(-1)（写 sig 与 erase 键），非复用局部 characNo。
        *reinterpret_cast<unsigned int*>(sig) = user->get_charac_no(-1);
        // [L3EHeroItemUpgrade 2026-09-11] ORIG 0x08166fda..0x08166fe8 在 strncpy
        // 实参处才 `mov -0xc(%ebp),%edx; add $0x4,%edx`（-0xc 即 sig 本身）⇒
        // 源码无具名 dst 局部（P183）。
        strncpy(reinterpret_cast<char*>(sig) + 4, data.c_str(), data.length());
        GlobalData::s_msgq_mgr->put(
            static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
        if (removeData)
        {
            m_values.erase(user->get_charac_no(-1));
        }
    }
}

