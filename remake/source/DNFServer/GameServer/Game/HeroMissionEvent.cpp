// ============================================================================
// df_game_r 还原 —— HeroMissionEvent（typo 批次，替换 GameStubs 桥接）。
// processMission @ 0x081672be / findMissionValue @ 0x08168010
// 忠实转写 docs/class_func_reports/HeroMissionEvent/{processMission,findMissionValue}.md。
// 判活：ORIG 经 vptr+0x34 调用 CEventBase::IsEventing(NULL)（即读取 +0x08 标志）。
// ============================================================================
// [b28d-lane15 集中判词] 本 TU 清单 19 件的 b28d 复核（fresh check_tu_routed：
//   COMPILE OK / identical=184 ae=331 near=1 diff=22）。逐件 N 归因（ORIG 地址实查）：
//   · _ZL29ConvertString2EnumMissionTypeRSs @0x089613d7 —— N（std::string 比较/构造
//     链 N89 族 + 局部槽组平移）。
//   · findMissionValue @0x08168010 —— N（N18b 成员==参数 mem-cmp 折叠；b28c-L7 已判）。
//   · {TotalMission@0x08166378 / UseCubeMission@0x08165992 / UseFatigueMission
//     @0x08165706 / AttendanceMission@0x08166002 / ClearDungeonMission@0x081653b4}
//     ::processMission —— N（Mission 族同构骨架：P168a 档① bool callee + P164
//     循环旋转 + N82 帧槽；b28c-L26/L27 已判）。
//   · {UseCubeMission@0x08165a2e / UseFatigueMission@0x081657a2 / AttendanceMission
//     @0x081661d6 / ClearDungeonMission@0x08165484}::rewardMission —— N（同骨架，
//     b28c-L26 判词：char 返回 callee 的 X1 边界 + bool 物化岛）。
//   · loadEventData@0x0816709e / saveEventData@0x08166d38 / setEventData@0x08166752 /
//     loadScript@0x08167ac8 —— N（N82 帧槽复用 + N89 STL 内联 + .o 重定位显示差）。
//   · HeroMissionEvent::processMission@0x081672be —— N（P164/P223 岛 + N82）。
//   · HeroMissionValue::getStringToken@0x08164bca —— N（std::string 族 N89）。
//   · LevelUpMission::processMission@0x08165c1e —— N（同 Mission 骨架 + N82）。
//   · importHeroMissionConditionsData@0x0896171b —— N（超大解析：N89 惰性 map 注册链
//     + P163 决策树 + N82；按 P165/P166 三件套口径收）。
//   语义均按报告逐指令转写并核对；残差全为跨版本结构族，无真语义缺口。
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

int importHeroMissionConditionsData(std::vector<HeroMissionCondition>* out,
                                    const char* path);

// ---- 显式实例化：ARAD::Singleton<HeroMissionEvent>::Get()（ORIG W 0x0811c9ce）----
namespace ARAD
{
template HeroMissionEvent* Singleton<HeroMissionEvent>::Get();
}

HeroMissionEvent::HeroMissionEvent()
{
    // ORIG ctor 0x08166522：Singleton 基类 + CEventBase 基类（stADDINFO 置
    // 0xffff、flag 置 0）自动构造；m_conditions / m_values 默认构造。
}

// ============================================================================
// findMissionValue @ 0x08168010
//   bool findMissionValue(CUser*, MissionNo::T, HeroMissionValue*&)
//   判活 → mission ∈ [1,6] → m_values[charac_no] 的 vector 中匹配 mission。
// ============================================================================
bool HeroMissionEvent::findMissionValue(CUser* user,
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
        // [b28c-L7] 负例：P209 提示的 signed 保形（`static_cast<int>` 双侧）实测
        // 反劣（101 vs 103，movsbl+双载 +2）；ORIG `mov (%eax),%eax;
        // cmp 0x10(%ebp),%eax` 的「值直落 eax + mem 作源」为 4.1 固有
        // （N18b：成员==参数 4.4 恒折 `cmp MEM,reg` 反向不可造）⇒ 封 N。
        if (vit->m_mission == static_cast<int>(mission))
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
void HeroMissionEvent::processMission(CUser* user,
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
    if (!findMissionValue(user, mission, value))
        return;

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
    if (mission == 4 && prev == 0 && value->m_fieldA != 0)
    {
        processMission(user, static_cast<HeroMissionCondition::MissionNo::T>(6), 1);
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
void HeroMissionEvent::loadEventData(CUser* user)
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

    Stream* stream = GlobalData::s_stream_pool->Acquire(
        "localtaiwan/Event/EventHeroMission.cpp", 0x291);
    CStreamGuard guard(stream, true);
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
        *reinterpret_cast<int*>(sig) = user->get_charac_no(-1);
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
void HeroMissionEvent::setMissionValue(CUser* user,
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
void HeroMissionEvent::StartEvent()
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
void HeroMissionEvent::StartEvent(Word_Param param)
{
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
    StartEvent();
}

// ============================================================================
// ORIG 0x08166700（EndEvent.md 全文已读）：先 cMyTrace("virtual void
// HeroMissionEvent::EndEvent()",0x20d,0)("[HeroMission] End!")，再
// SetEventFlag(false)。镜像布局以 m_isEventing=0 等价。
// ============================================================================
void HeroMissionEvent::EndEvent()
{
    cMyTrace tr(__PRETTY_FUNCTION__, 0x20d, 0);
    tr("[HeroMission] End!");
    // ORIG 0x816674a：SetEventFlag(false)（写 +0x08 标志，与 IsEventing 同字段）
    SetEventFlag(false);
}

TotalMission::TotalMission()
    : BaseHeroMissionCondition(HeroMissionCondition::MissionNo::T_6)
{
}
TotalMission::~TotalMission() {}
// ============================================================================
// TotalMission::processMission —— ORIG T 0x08166378（processMission.md 全文转写）。
// 合计任务推进：value.m_field4 按 param 累加，以 m_condition.m_extra.size()
// 为上限（区别于其它 mission 用 m_extra[0]）。
// ============================================================================
void TotalMission::processMission(CUser* /*user*/, HeroMissionValue& value,
                                  unsigned int param)
{
    if (value.m_field4 < m_condition.m_extra.size())
    {
        if (value.m_field4 + param < m_condition.m_extra.size())
        {
            value.m_field4 = value.m_field4 + static_cast<unsigned short>(param);
        }
        else
        {
            value.m_field4 =
                static_cast<unsigned short>(m_condition.m_extra.size());
        }
    }
}


ClearDungeonMission::ClearDungeonMission()
    : BaseHeroMissionCondition(HeroMissionCondition::MissionNo::T_1)
{
}
ClearDungeonMission::~ClearDungeonMission() {}
// ============================================================================
// ClearDungeonMission::processMission —— ORIG T 0x081653b4（processMission.md）。
// 清图计数推进：m_extra[0] 为上限；missionType==2 且 field8==7 的任务不推进
// （特殊关卡类型屏蔽）。
// ============================================================================
void ClearDungeonMission::processMission(CUser* /*user*/, HeroMissionValue& value,
                                         unsigned int param)
{
    if (value.m_field4 < m_condition.m_extra[0])
    {
        if (!(getMissionType() == 2 && value.m_field8 != 7))
        {
            if (value.m_field4 + param < m_condition.m_extra[0])
            {
                value.m_field4 =
                    value.m_field4 + static_cast<unsigned short>(param);
            }
            else
            {
                value.m_field4 = m_condition.m_extra[0];
            }
        }
    }
}

// ============================================================================
// ClearDungeonMission::rewardMission —— ORIG T 0x08165484（rewardMission.md 全文）。
// 判活顺序：已奖励（非重复且 fieldA!=0）→ 任务条件不满足（type==2 且
// field8!=7）→ 计数未达上限 → rewardProcess 发奖并 fieldA++/field6=1，
// 重复任务 reset。
// ============================================================================
bool ClearDungeonMission::rewardMission(CUser* user, HeroMissionValue& value)
{
    if (!isRepeatMission() && value.m_fieldA != 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0xac,
            "[HeroMission] Already reward. (charac:%u,mission:%d,count:%d)",
            user->get_charac_no(-1), value.m_mission, value.m_fieldA);
        return false;
    }
    if (getMissionType() == 2 && value.m_field8 != 7)
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0xb4,
            "[HeroMission] Invalid reward condition. (charac:%u,mission:%d,value:%d)",
            user->get_charac_no(-1), value.m_mission, value.m_field8);
        return false;
    }
    if (value.m_field4 < m_condition.m_extra[0])
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0xbb,
            "[HeroMission] Invalid reward condition. (charac:%u,mission:%d,count:%d)",
            user->get_charac_no(-1), value.m_mission, value.m_fieldA);
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

UseFatigueMission::UseFatigueMission()
    : BaseHeroMissionCondition(HeroMissionCondition::MissionNo::T_2)
{
}
UseFatigueMission::~UseFatigueMission() {}
// ============================================================================
// UseFatigueMission::processMission —— ORIG T 0x08165706（processMission.md）。
// 疲劳消耗推进：m_extra[0] 为上限，field4+param 超限则钳制到上限。
// ============================================================================
void UseFatigueMission::processMission(CUser* /*user*/, HeroMissionValue& value,
                                       unsigned int amount)
{
    if (value.m_field4 < m_condition.m_extra[0])
    {
        if (value.m_field4 + amount < m_condition.m_extra[0])
        {
            value.m_field4 = value.m_field4 + static_cast<unsigned short>(amount);
        }
        else
        {
            value.m_field4 = m_condition.m_extra[0];
        }
    }
}

// ============================================================================
// UseFatigueMission::rewardMission —— ORIG T 0x081657a2（rewardMission.md）。
// 同 ClearDungeonMission::rewardMission 骨架（无 missionType 屏蔽分支）。
// ============================================================================
bool UseFatigueMission::rewardMission(CUser* user, HeroMissionValue& value)
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
            user->get_charac_no(-1), value.m_mission, value.m_fieldA);
        return false;
    }
    if (value.m_field4 < m_condition.m_extra[0])
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0xec,
            "[HeroMission] Invalid reward condition. (charac:%u,mission:%d,count:%d)",
            user->get_charac_no(-1), value.m_mission, value.m_fieldA);
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

UseCubeMission::UseCubeMission()
    : BaseHeroMissionCondition(HeroMissionCondition::MissionNo::T_3)
{
}
UseCubeMission::~UseCubeMission() {}
// ============================================================================
// UseCubeMission::processMission —— ORIG T 0x08165992（processMission.md）。
// 魔盒消耗推进：同 UseFatigueMission 骨架（m_extra[0] 上限钳制）。
// ============================================================================
void UseCubeMission::processMission(CUser* /*user*/, HeroMissionValue& value,
                                    unsigned int amount)
{
    if (value.m_field4 <= m_condition.m_extra[0])
    {
        if (value.m_field4 + amount <= m_condition.m_extra[0])
        {
            value.m_field4 = value.m_field4 + static_cast<unsigned short>(amount);
        }
        else
        {
            value.m_field4 = m_condition.m_extra[0];
        }
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
bool UseCubeMission::rewardMission(CUser* user, HeroMissionValue& value)
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
    const bool alreadyRewarded = !isRepeatMission() && value.m_fieldA != 0;
    if (alreadyRewarded)
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0x117,
            "[HeroMission] Already reward. (charac:%u,mission:%d,count:%d)",
            user->get_charac_no(-1), value.m_mission, value.m_fieldA);
        return false;
    }
    if (value.m_field4 < m_condition.m_extra[0])
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__,
            0x11e,
            "[HeroMission] Invalid reward condition. (charac:%u,mission:%d,count:%d)",
            user->get_charac_no(-1), value.m_mission, value.m_fieldA);
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

LevelUpMission::LevelUpMission()
    : BaseHeroMissionCondition(HeroMissionCondition::MissionNo::T_4)
{
}
LevelUpMission::~LevelUpMission() {}
// ============================================================================
// LevelUpMission::processMission —— ORIG T 0x08165c1e（processMission.md 全文）。
// 升级任务：m_extra[0] 为等级上限，field8 记录上次等级；达到新等级时遍历
// m_rewards 逐项构造 Inven_Item 并以系统邮件发放，随后 fieldA++/field6=1、
// field8 记当前等级，重复任务 reset。
// ============================================================================
void LevelUpMission::processMission(CUser* user, HeroMissionValue& value,
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
            user->get_charac_no(-1), value.m_mission, value.m_field8);
        return;
    }

    std::vector<HeroMissionCondition::RewardData>::iterator it =
        m_condition.m_rewards.begin();
    std::vector<HeroMissionCondition::RewardData>::iterator end =
        m_condition.m_rewards.end();
    for (; it != end; ++it)
    {
        CItem* item = G_CDataManager()->find_item(it->m_itemIdx);
        if (item == 0)
        {
            cMyTrace tr(__PRETTY_FUNCTION__,
                        0x150, 5);
            tr("[HeroMission] Invalid item. (charac:%u, mission:%d, item:%u)",
               user->get_charac_no(-1), m_missionCode, it->m_itemIdx);
            return;
        }
        Inven_Item mailItem;
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
bool LevelUpMission::rewardMission(CUser* /*user*/, HeroMissionValue& /*value*/)
{
    return true;
}

AttendanceMission::AttendanceMission()
    : BaseHeroMissionCondition(HeroMissionCondition::MissionNo::T_5)
{
}
AttendanceMission::~AttendanceMission() {}
// ============================================================================
// AttendanceMission::processMission —— ORIG T 0x08166002（processMission.md）。
// 签到：以 fatigueTime 为每日重置时刻；m_extra[0] 为签到上限、[1] 为开始
// 月份、[2] 为开始日；与目标日相差超过 1 天（0x15180 秒）先 reset，再
// field4+1（钳制到上限）并 field8 累加相差天数。
// ============================================================================
void AttendanceMission::processMission(CUser* /*user*/, HeroMissionValue& value,
                                       unsigned int /*param*/)
{
    if (value.m_field4 < m_condition.m_extra[0])
    {
                struct tm target;      // ORIG -0x6c（先声明占低地址）
        struct tm today;       // ORIG -0x40
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
        if (value.m_field4 + 1 < m_condition.m_extra[0])
        {
            value.m_field4 = value.m_field4 + 1;
        }
        else
        {
            value.m_field4 = m_condition.m_extra[0];
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
bool AttendanceMission::rewardMission(CUser* user, HeroMissionValue& value)
{
    if (!isRepeatMission() && value.m_fieldA != 0)
    {
        return false;
    }
    if (value.m_field4 < m_condition.m_extra[0])
    {
        cMyTrace tr(__PRETTY_FUNCTION__,
                    0x1a4, 5);
        tr("[HeroMission] Reward condition error. (charac:%u,mission:%d,count:%d)",
           user->get_charac_no(-1), value.m_mission, value.m_fieldA);
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
// ============================================================================
static int ConvertString2EnumMissionType(std::string& s)
{
loop_top:
    static std::map<const char*, int, less_TCHAR> lookup;
    static SpinLock lock;
    static bool inited;
    int code;
    if (inited)
    {
        std::map<const char*, int, less_TCHAR>::const_iterator it =
            lookup.find(s.c_str());
        code = (it != lookup.end()) ? it->second : -1;
    }
    else
    {
        lock.enter();
        if (inited)
        {
            lock.leave();
            goto loop_top;
        }
        code = 0;
    }

    switch (code)
    {
    case 0:
    case 0x3d1:   // "normal"
        if (!inited)
            lookup.insert(std::pair<const char* const, int>("normal", 0x3d1));
        else
            return 1;
    case 0x3d3:   // "payment"
        if (!inited)
            lookup.insert(std::pair<const char* const, int>("payment", 0x3d3));
        else
            return 2;
    case 0x3d5:   // "final"
        if (!inited)
            lookup.insert(std::pair<const char* const, int>("final", 0x3d5));
        else
            return 3;
    }

    if (!inited)
    {
        lock.leave();
        inited = true;
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
// 首轮经 switch fallthrough 链惰性填充标签表（lookup @0x94f7578 / SpinLock
// @0x94f7590 / 就绪字节 @0x94f7594，P21 范式），随后重查当前行分派。
// ============================================================================
int importHeroMissionConditionsData(std::vector<HeroMissionCondition>* out,
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

bool HeroMissionEvent::loadScript()
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
                std::map<HeroMissionCondition::MissionNo::T, BaseHeroMissionCondition*>::iterator mit =
                    m_conditions.find(code);
                if (mit != m_conditions.end() && mit->second != 0)
                    mit->second->setConditionData(*it);
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
void HeroMissionEvent::setEventData(CUser* user, char const* data)
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
            value.setStringToken(*tit);
            values.push_back(value);
        }
    }

    unsigned int characNo = (unsigned int)user->get_charac_no(-1);
    std::map<unsigned int, std::vector<HeroMissionValue> >::iterator it =
        m_values.find(characNo);
    if (it != m_values.end())
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__, 0x24b,
            "[HeroMission] Already exist character data. (charac:%u)",
            characNo);
        m_values.erase(characNo);
    }
    m_values.insert(std::make_pair(characNo, values));
}
// ============================================================================
// HeroMissionValue::getStringToken —— ORIG T 0x08164bca（getStringToken.md
// 全文转写；saveEventData 依赖，定义归本 TU）。
// 若 out 非空先补 ','，再按 "mission f4 f6 fA f8" 空格分隔追加（字段序与
// setStringToken 的解析偏移一一对应：tok0→m_mission、tok1→+0x04、tok2→+0x06、
// tok3→+0x0a、tok4→+0x08）。
// ============================================================================
void HeroMissionValue::getStringToken(std::string& out)
{
    std::stringstream ss;
    if (out.length() != 0)
    {
        ss << ',';
    }
    ss << m_mission << ' ' << m_field4 << ' ' << m_field6 << ' '
       << m_fieldA << ' ' << m_field8;
    out += ss.str();
}
// ============================================================================
// setEventData(std::string const&) —— ORIG T 0x08166d0e（setEventData_08166d0e.md
// 全文转写）：thunk——c_str() 后转调 char const* 版本。
// ============================================================================
void HeroMissionEvent::setEventData(CUser* user, const std::string& data)
{
    setEventData(user, data.c_str());
}
// ============================================================================
// setMissionForGm —— ORIG T 0x081679ac（setMissionForGm.md 全文转写）。
// findMissionValue 命中且 m_conditions 中存在该 mission 时，
// 写 HeroMissionValue +0x04（m_field4）为 ushort 截断的 GM 设定值。
// ============================================================================
void HeroMissionEvent::setMissionForGm(CUser* user,
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
void HeroMissionEvent::resetMissionForGm(CUser* user,
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
void HeroMissionEvent::saveEventData(CUser* user, bool removeData)
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

    unsigned int characNo = (unsigned int)user->get_charac_no(-1);
    std::map<unsigned int, std::vector<HeroMissionValue> >::iterator it =
        m_values.find(characNo);
    if (it == m_values.end())
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__, 0x269,
            "[HeroMission] Not exist mission value. (charac:%u)", characNo);
        return;
    }

    std::string data;
    for (std::vector<HeroMissionValue>::iterator vit = it->second.begin();
         vit != it->second.end(); ++vit)
    {
        vit->getStringToken(data);
    }

    Stream* stream = GlobalData::s_stream_pool->Acquire(
        "localtaiwan/Event/EventHeroMission.cpp", 0x271);
    CStreamGuard guard(stream, true);
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
        *reinterpret_cast<unsigned int*>(sig) = characNo;
        char* dst = reinterpret_cast<char*>(sig) + 4;
        strncpy(dst, data.c_str(), data.length());
        GlobalData::s_msgq_mgr->put(
            static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
        if (removeData)
        {
            m_values.erase(characNo);
        }
    }
}

