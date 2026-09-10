// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 AradPeriodB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include <cstring>

#include "Arad_EventPeriodDataManager.h" // 类主布局 / EventRewardItemArad /
                                         // SearchEventPeriod / SRewardPeriodData
#include "Arad_DataManager.h"            // ARAD::PRINT_ERROR
#include "MoreTypes.h"                   // arad::SigSaveRewardEventItem
#include "CUser.h"                       // get_charac_no / GetUID / GetServerGroup /
                                         // get_acc_id
#include "Inven_Item.h"                  // Inven_Item::reset
#include "CDataManager.h"                // G_CDataManager / find_item
#include "CMailBoxHelper.h"              // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "RDARScriptStringManager.h"     // g_scriptStringManager_ / findString
#include "GlobalData.h"                  // s_stream_pool / s_msgq_mgr
#include "LogManager.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "Arad_InternalDispatcher.h"     // DISPATCHER::make_internal_stream_jpn

// ---------------------------------------------------------------------------
// 【A 档 GI 顺序对齐 2026-09-09】FnRewardInterface 类与本类 operator() 定义前置，
// 使 `_GLOBAL__I_` 名等于 ORIG 的 `_ZN4ARAD17FnRewardInterfaceclEimsj`（P327）。
// make_data 仍留在下方命名空间块（类已在此声明）。语义与机器码零变化，仅发射序。
// ---------------------------------------------------------------------------
namespace ARAD { namespace ARAD_ITEM {
#include "AradPeriodB/arad_make_item.inc"
} }

namespace ARAD {

// ---- FnRewardInterface -----------------------------------------------------
class FnRewardInterface
{
public:
    inline FnRewardInterface(CUser* user, bool recordCharacNo)   // ORIG W 0x0819a8c8
        : m_user(user), m_recordCharacNo(recordCharacNo)
    {
    }

    virtual void GetMailString(char* mailBuf, char* titleBuf) = 0;

    EventRewardItemArad make_data(int eventCode);              // T 0x08194e58
    bool operator()(int eventCode, unsigned long itemIndex,
                    short count, unsigned int param);          // T 0x08194ac8

protected:
    CUser* m_user;
    bool   m_recordCharacNo;
};

#include "AradPeriodB/operator.inc"
                                   short count, unsigned int param)
{
    if (m_user == 0 || m_user->get_charac_no(-1) == 0)
        return false;

    Inven_Item item;
    if (!ARAD_ITEM::arad_make_item(itemIndex, (int)count, item))
        return false;

    char mailBuf[0x100] = {0};
    char titleBuf[0x15] = {0};
    GetMailString(mailBuf, titleBuf);

    int mailRet = WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        titleBuf, item, 0, m_user->get_charac_no(-1), mailBuf,
        (int)std::strlen(mailBuf), 0xc, m_user->GetServerGroup(), false,
        false);
    if (mailRet != 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_EventPeriodDataManager.cpp",
            __PRETTY_FUNCTION__,
            0x2f,
            "[FnReward] send mail fail. (code:%u, charac:%u, item:%u, cnt:%d)",
            eventCode, m_user->get_charac_no(-1), itemIndex, (int)count);
        return false;
    }

    EventRewardItemArad data = make_data(eventCode);
    arad::SigSaveRewardEventItem sig;
    sig.set(eventCode, data.m_accId, data.m_characNo, itemIndex, count, param);
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire(
            "localjapan/Arad_EventPeriodDataManager.cpp", 0x36),
        true);
    DISPATCHER::make_internal_stream_jpn(guard,
                                         (ENUM_INTERNALPACKET_JPN)0x27,
                                         (int)m_user->GetUID());
    guard->put_binary(&sig, 0x16);
    return GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), guard);
}

}  // namespace ARAD

// ============================================================================
// 【报告缺失,objdump】ARAD::ARAD_ITEM::arad_make_item —— ORIG T 0x08197375
// (133B)：idx 为 0/-1 或 count==0 返 false；find_item(idx) 取物品；out.reset()
// 后 vtable+8 make_item(out) 填充；再写 m_addInfo(+2)=idx、m_addInfo2(+7)=count。
// ============================================================================
namespace ARAD {
namespace ARAD_ITEM {

#include "AradPeriodB/arad_make_item_1.inc"
{
    if (itemIndex == 0 || (int)itemIndex == -1 || count == 0)
        return false;
    CItem* item = G_CDataManager()->find_item((int)itemIndex);
    if (item == 0)
        return false;
    out.reset();
    item->make_item(out);
    out.m_addInfo  = (int)itemIndex;   // +0x02（ORIG mov %edx,0x2(%eax)）
    out.m_addInfo2 = count;            // +0x07（ORIG mov %edx,0x7(%eax)）
    return true;
}

}  // namespace ARAD_ITEM

// ============================================================================
// 【报告缺失,objdump】PRINT_ERROR::find_item —— ORIG T 0x08197caf。
// "%s : find_item() fail [item_id:%d] [charac_no:%d]"
// file=localjapan/Arad_Function_Set.cpp line=0x2ac（归属建议：并入
// Arad_Function_Set.cpp 同族段；暂放本 TU 保证链接）。
// ============================================================================
#include "AradPeriodB/find_item.inc"
{
    LogManager::logFormat(
        1, "localjapan/Arad_Function_Set.cpp",
        __PRETTY_FUNCTION__,
        0x2ac,
        "%s : find_item() fail [item_id:%d] [charac_no:%d]",
        tag, itemId, (unsigned int)user->getCurCharacNo());
}

}  // namespace ARAD

// ============================================================================
// 【报告缺失,objdump】SRewardPeriodData ctor —— ORIG T 0x088b439c
// C1(i,m,s,j,j)：五字段直存（+0/+4/+8 word/+0xc/+0x10）。
// ============================================================================
#include "AradPeriodB/SRewardPeriodData.inc"
    int eventCode, unsigned long f4, short f8,
    unsigned int beginTime, unsigned int endTime)
{
    m_eventCode = eventCode;
    m_field4    = (unsigned int)f4;
    m_field8    = f8;
    m_beginTime = beginTime;
    m_endTime   = endTime;
}

namespace ARAD {

// ============================================================================
// FnReward 函数对象族 —— 布局由 ORIG C1/W 反汇编实证：
//   FnRewardInterface{ vptr; +4 CUser*; +8 bool }（C1 0x0819a8c8：vptr→user→bool）
//   FnRewardMomoji : FnRewardInterface（C1 0x0819a900：基(b=true)后再写自身 vptr；
//                    ZTV 只有槽0=GetMailString，MakeMailString 为唯一虚函数）
//   FnRewardLethe 非多态独立类 { +0 CUser* }（C1 0x0819a8f2 仅存指针）
// ============================================================================

#include "AradPeriodB/make_data.inc"
{
    if (m_recordCharacNo != 0)
    {
        return EventRewardItemArad(eventCode, m_user->get_acc_id(),
                                   m_user->get_charac_no(-1));
    }
    return EventRewardItemArad(eventCode, m_user->get_acc_id(), 0);
}

class FnRewardMomoji : public FnRewardInterface
{
public:
    inline explicit FnRewardMomoji(CUser* user)                 // ORIG W 0x0819a900
        : FnRewardInterface(user, true)
    {
    }

    virtual void GetMailString(char* mailBuf, char* titleBuf);
};

}  // namespace ARAD

// ORIG T 0x0819537c（报告 ARAD__FnRewardMomoji/GetMailString.md 全文转写）。
#include "AradPeriodB/GetMailString.inc"
{
    std::strncpy(titleBuf, g_scriptStringManager_.findString(4, "momiji_event_title", (bool*)0), 0x14);
    std::strncpy(mailBuf, g_scriptStringManager_.findString(4, "momiji_event_mail", (bool*)0), 0xff);
}

namespace ARAD {

// ---- FnRewardLethe ---------------------------------------------------------
class FnRewardLethe
{
public:
    inline explicit FnRewardLethe(CUser* user)                  // ORIG W 0x0819a8f2
        : m_user(user)
    {
    }

    EventRewardItemArad make_data(int eventCode);              // T 0x08195340
    bool operator()(int eventCode, unsigned long itemIndex,
                    short count, unsigned int param);          // T 0x08194ee2

private:
    CUser* m_user;
};

#include "AradPeriodB/make_data_1.inc"
{
    return EventRewardItemArad(eventCode, m_user->get_acc_id(), 0);
}

// [b28d-solo-AradPeriodB] 判词：**AE**（check_tu routed；284=284；p167 30=30 CLEAR）。
// 修法：同 FnRewardInterface::operator()（数组初始化/内联实参/operator-> 链/早返布局）；
// 另把 title/mail 两支的 strncpy 内联进各分支（删 const char* 具名局部，P0 槽-局部律，
// 帧 -8 后 mailBuf/titleBuf 槽位与 ORIG 全等）。
// ORIG T 0x08194ee2（报告 ARAD__FnRewardLethe/operator__.md 全文转写）。
#include "AradPeriodB/operator_1.inc"
                               short count, unsigned int param)
{
    if (m_user == 0 || m_user->get_charac_no(-1) == 0)
        return false;

    Inven_Item item;
    if (!ARAD_ITEM::arad_make_item(itemIndex, (int)count, item))
        return false;

    char mailBuf[0x100] = {0};
    char titleBuf[0x15] = {0};
    // ORIG：param_1==0x60 用 soccer 金牌字符串，否则 lethe 夏日活动串。
    if (eventCode == 0x60)
    {
        std::strncpy(titleBuf,
                     g_scriptStringManager_.findString(
                         4, "2012_soccer_goldmedal_title", (bool*)0),
                     0x14);
        std::strncpy(mailBuf,
                     g_scriptStringManager_.findString(
                         4, "2012_soccer_goldmedal_mail", (bool*)0),
                     0xff);
    }
    else
    {
        std::strncpy(titleBuf,
                     g_scriptStringManager_.findString(
                         4, "lethe_2012_summer_event_title", (bool*)0),
                     0x14);
        std::strncpy(mailBuf,
                     g_scriptStringManager_.findString(
                         4, "lethe_2012_summer_event_mail", (bool*)0),
                     0xff);
    }

    int mailRet = WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        titleBuf, item, 0, m_user->get_charac_no(-1), mailBuf,
        (int)std::strlen(mailBuf), 0xc, m_user->GetServerGroup(), false,
        false);
    if (mailRet != 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_EventPeriodDataManager.cpp",
            __PRETTY_FUNCTION__,
            0x66,
            "[RewardLethe] send mail fail. (charac:%u, item:%u, cnt:%d)",
            m_user->get_charac_no(-1), itemIndex, (int)count);
        return false;
    }

    EventRewardItemArad data = make_data(eventCode);
    arad::SigSaveRewardEventItem sig;
    sig.set(eventCode, data.m_accId, data.m_characNo, itemIndex, count, param);
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire(
            "localjapan/Arad_EventPeriodDataManager.cpp", 0x6d),
        true);
    DISPATCHER::make_internal_stream_jpn(guard,
                                         (ENUM_INTERNALPACKET_JPN)0x27,
                                         (int)m_user->GetUID());
    guard->put_binary(&sig, 0x16);
    return GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), guard);
}

}  // namespace ARAD

// ============================================================================
// ARAD::Arad_EventPeriodDataManager::eventReward<F> 模板主函数 +
// 两个显式实例化（ORIG W 0x0819abc8 / 0x0819adb6，各 493B 逐指令等价转写）。
// [b28d-solo-AradPeriodB] 判词：**AE**（check_tu routed；两实例各 147=147；
// p167 17=17 CLEAR ×2）。
// 修法：入口守卫合并短路物化岛（P214①）、it!=end/时间窗/fn() 正向包裹共享尾块
// （P222/P214①）、time 具名缓存槽 -0xc（P0）。
// 槽序翻正关键：pred 仅作 find_if 第三实参 ⇒ 去具名局部、内联匿名临时
// `SearchEventPeriod(eventCode)`，4.4 即把 fn/data/pred 排成 ORIG 的 pred -0x1c /
// fn -0x20 / data -0x24..-0x2c（4.1 先声明深方向）。具名 pred 时 4.4 恒先声明浅
// （fn -0x1c/pred -0x2c）⇒ NEAR；翻声明序 NEAR→DIFF 劣化。
// 新范式候选（P0 右值内联 + P212 槽序）：谓词类局部若仅作单次调用实参，内联为
// 匿名临时可同时消槽并把同块具名对象槽序翻回 ORIG 方向。
// ============================================================================
template <typename F>
#include "AradPeriodB/eventReward.inc"
                                                    unsigned int time,
                                                    CUser* user)
{
    unsigned int curTime;      // ORIG 0x819acfa：time 具名缓存槽（-0xc）

    // 入口门禁：user 空 或 当前角色编号为 0 → 直接失败返 false（ORIG 0xabd0..0xac07）。
    if (user == 0 || user->get_charac_no(-1) == 0)
        return false;

    F fn(user);                                        // ORIG call C1(user)
    EventRewardItemArad data = fn.make_data((int)eventCode);
    if (isRewarded(data))                              // ORIG 0x081958b6（按值传参）
        return false;

    typename std::vector<SCRIPT::SRewardPeriodData>::iterator it =
        std::find_if(m_rewardPeriodList.begin(), m_rewardPeriodList.end(),
                     SearchEventPeriod(eventCode));    // ORIG SearchEventPeriodC1
    // ORIG 生成 __gnu_cxx::operator!=（find_if 尾部 it != end 判定）
    if (it != m_rewardPeriodList.end())
    {
        curTime = time;
        // ORIG 0x819ad0b：elem+0xc(begin) <= time && elem+0x10(end) > time
        if (it->m_beginTime <= curTime && it->m_endTime > curTime)
        {
            // ORIG 0x819ad77：fn(time, elem+4(itemIndex), sw(elem+8)(count), elem+10(endTime))
            if (fn((int)eventCode, it->m_field4, it->m_field8, it->m_endTime))
            {
                addUserRewardData(user, data);         // ORIG 0x08195aa4（引用重载）
                return true;
            }
        }
    }
    return false;
}

template bool ARAD::Arad_EventPeriodDataManager::eventReward<ARAD::FnRewardLethe>(
    ENUM_REPEAT_EVENT_CODE, unsigned int, CUser*);
template bool ARAD::Arad_EventPeriodDataManager::eventReward<ARAD::FnRewardMomoji>(
    ENUM_REPEAT_EVENT_CODE, unsigned int, CUser*);

// ============================================================================
// ARAD::SCRIPT::AradJumping_Script::getCurRewardItem —— ORIG T 0x088b424a
//（报告 ARAD__SCRIPT__AradJumping_Script/getCurRewardItem.md 全文转写）：
// 线性扫 m_rewardItems；(m_job==job && m_growValue==growValue) → out=m_money、
// 返回 true；未命中返回 false。
// ORIG nm 为 `_ZN`（非 const 成员，无 `_ZNK` 版本）；头声明已同步去 const。
// ============================================================================
#include "AradPeriodB/getCurRewardItem.inc"
                                                        unsigned long& out)
{
    std::vector<ARAD::SCRIPT::_AradJumpingRewardItem>::iterator it =
        m_rewardItems.begin();
    std::vector<ARAD::SCRIPT::_AradJumpingRewardItem>::iterator end =
        m_rewardItems.end();
    while (it != end)
    {
        if (it->m_job == job && it->m_growValue == growValue)
        {
            out = it->m_money;
            break;
        }
        ++it;
    }
    if (it != end)
        return true;
    return false;
}
