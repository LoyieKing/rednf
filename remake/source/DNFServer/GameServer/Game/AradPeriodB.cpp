// ============================================================================
// Game/AradPeriodB.cpp —— LinkClosureB.AradServers 独占 TU（批 B）。
//
// 覆盖 ORIG 符号（df_game_r nm 实证）：
//   _ZN4ARAD9ARAD_ITEM14arad_make_itemEmiR10Inven_Item               T 0x08197375
//   _ZN4ARAD11PRINT_ERROR9find_itemEPcP5CUseri                        T 0x08197caf
//   _ZNK4ARAD6SCRIPT18AradJumping_Script16getCurRewardItemEiiRm       T 0x088b424a
//   _ZN4ARAD27Arad_EventPeriodDataManager11eventRewardINS_13FnRewardLetheEEEb22ENUM_REPEAT_EVENT_CODEjP5CUser  W 0x0819abc8
//   _ZN4ARAD27Arad_EventPeriodDataManager11eventRewardINS_14FnRewardMomojiEEEb22ENUM_REPEAT_EVENT_CODEjP5CUser W 0x0819adb6
//   _ZN4ARAD13FnRewardLetheC1EP5CUser                                 W 0x0819a8f2（inline）
//   _ZN4ARAD13FnRewardLethe9make_dataEi                               T 0x08195340
//   _ZN4ARAD13FnRewardLetheclEimsj                                    T 0x08194ee2
//   _ZN4ARAD17FnRewardInterfaceC1EP5CUserb                            W 0x0819a8c8（inline）
//   _ZN4ARAD17FnRewardInterface9make_dataEi                           T 0x08194e58
//   _ZN4ARAD17FnRewardInterfaceclEimsj                                T 0x08194ac8
//   _ZN4ARAD14FnRewardMomojiC1EP5CUser                                W 0x0819a900（inline）
//   _ZN4ARAD14FnRewardMomoji13GetMailStringEPcS1_                     T 0x0819537c
//   + FnRewardInterface/FnRewardMomoji 的 ZTV/ZTI/ZTS（Momoji 带虚 GetMailString），
//     SRewardPeriodDataC1Eimsjj（0x088b439c）与 vector<SRewardPeriodData>
//     begin/end/find_if 等模板支撑由本 TU 的定义自动产出。
//
// 证据来源：
//   docs/class_func_reports/ARAD__SCRIPT__AradJumping_Script/getCurRewardItem.md
//   docs/class_func_reports/ARAD__FnRewardLethe/{README,make_data,operator__}.md
//   docs/class_func_reports/ARAD__FnRewardMomoji/{README,GetMailString}.md
//   （无报告三处——arad_make_item/PRINT_ERROR::find_item/SRewardPeriodDataC1
//     —— 以 objdump 反汇编逐指令转写，标注【报告缺失,objdump】。）
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

// ============================================================================
// 【报告缺失,objdump】ARAD::ARAD_ITEM::arad_make_item —— ORIG T 0x08197375
// (133B)：idx 为 0/-1 或 count==0 返 false；find_item(idx) 取物品；out.reset()
// 后 vtable+8 make_item(out) 填充；再写 m_addInfo(+2)=idx、m_addInfo2(+7)=count。
// ============================================================================
namespace ARAD {
namespace ARAD_ITEM {

bool arad_make_item(unsigned long itemIndex, int count, Inven_Item& out)
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
void PRINT_ERROR::find_item(char* tag, CUser* user, int itemId)
{
    unsigned int characNo = (unsigned int)user->getCurCharacNo();
    LogManager::logFormat(
        1, "localjapan/Arad_Function_Set.cpp",
        __PRETTY_FUNCTION__,
        0x2ac,
        "%s : find_item() fail [item_id:%d] [charac_no:%d]",
        tag, itemId, characNo);
}

}  // namespace ARAD

// ============================================================================
// 【报告缺失,objdump】SRewardPeriodData ctor —— ORIG T 0x088b439c
// C1(i,m,s,j,j)：五字段直存（+0/+4/+8 word/+0xc/+0x10）。
// ============================================================================
ARAD::SCRIPT::SRewardPeriodData::SRewardPeriodData(
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
    ~FnRewardInterface() {}   // 保护非虚析构：避免编译器补虚析构槽（ORIG vtable 仅 1 槽）

    CUser* m_user;
    bool   m_recordCharacNo;
};

EventRewardItemArad FnRewardInterface::make_data(int eventCode)
{
    if (m_recordCharacNo != 0)
    {
        return EventRewardItemArad(eventCode, m_user->get_acc_id(),
                                   m_user->get_charac_no(-1));
    }
    return EventRewardItemArad(eventCode, m_user->get_acc_id(), 0);
}

bool FnRewardInterface::operator()(int eventCode, unsigned long itemIndex,
                                   short count, unsigned int param)
{
    if (m_user == 0 || m_user->get_charac_no(-1) == 0)
        return false;

    Inven_Item item;
    if (!ARAD_ITEM::arad_make_item(itemIndex, (int)count, item))
        return false;

    char mailBuf[0x100];
    char titleBuf[0x15];
    std::memset(mailBuf, 0, sizeof(mailBuf));
    std::memset(titleBuf, 0, sizeof(titleBuf));
    GetMailString(mailBuf, titleBuf);

    ENUM_SERVER_GROUP group = m_user->GetServerGroup();
    unsigned int characNo = m_user->get_charac_no(-1);
    int mailRet = WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        titleBuf, item, 0, characNo, mailBuf, (int)std::strlen(mailBuf),
        0xc, group, false, false);
    if (mailRet == 0)
    {
        EventRewardItemArad data = make_data(eventCode);
        arad::SigSaveRewardEventItem sig;
        sig.set(eventCode, data.m_accId, data.m_characNo, itemIndex, count,
                param);
        Stream* stream =
            GlobalData::s_stream_pool->Acquire("localjapan/Arad_EventPeriodDataManager.cpp",
                                               0x36);
        CStreamGuard guard(stream, true);
        DISPATCHER::make_internal_stream_jpn(guard,
                                             (ENUM_INTERNALPACKET_JPN)0x27,
                                             (int)m_user->GetUID());
        guard.put_binary(&sig, 0x16);
        MsgQueueMgr::QUEUE_IDX idx = MsgQueueMgr::QUEUE_IDX(2);
        GlobalData::s_msgq_mgr->put(idx, guard);
    }
    else
    {
        LogManager::logFormat(
            1, "localjapan/Arad_EventPeriodDataManager.cpp",
            __PRETTY_FUNCTION__,
            0x2f,
            "[FnReward] send mail fail. (code:%u, charac:%u, item:%u, cnt:%d)",
            (unsigned int)eventCode, characNo, (unsigned int)itemIndex,
            (int)count);
        return false;
    }
    return true;
}

// ---- FnRewardMomoji --------------------------------------------------------
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
void ARAD::FnRewardMomoji::GetMailString(char* mailBuf, char* titleBuf)
{
    const char* title = g_scriptStringManager_.findString(4, "momiji_event_title", (bool*)0);
    std::strncpy(titleBuf, title, 0x14);
    const char* mail = g_scriptStringManager_.findString(4, "momiji_event_mail", (bool*)0);
    std::strncpy(mailBuf, mail, 0xff);
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

EventRewardItemArad FnRewardLethe::make_data(int eventCode)
{
    return EventRewardItemArad(eventCode, m_user->get_acc_id(), 0);
}

// ORIG T 0x08194ee2（报告 ARAD__FnRewardLethe/operator__.md 全文转写）。
bool FnRewardLethe::operator()(int eventCode, unsigned long itemIndex,
                               short count, unsigned int param)
{
    if (m_user == 0 || m_user->get_charac_no(-1) == 0)
        return false;

    Inven_Item item;
    if (!ARAD_ITEM::arad_make_item(itemIndex, (int)count, item))
        return false;

    char mailBuf[0x100];
    char titleBuf[0x15];
    std::memset(mailBuf, 0, sizeof(mailBuf));
    std::memset(titleBuf, 0, sizeof(titleBuf));
    // ORIG：param_1==0x60 用 soccer 金牌字符串，否则 lethe 夏日活动串。
    const char* title;
    const char* mail;
    if (eventCode == 0x60)
    {
        title = g_scriptStringManager_.findString(4, "2012_soccer_goldmedal_title", (bool*)0);
        mail  = g_scriptStringManager_.findString(4, "2012_soccer_goldmedal_mail", (bool*)0);
    }
    else
    {
        title = g_scriptStringManager_.findString(4, "lethe_2012_summer_event_title", (bool*)0);
        mail  = g_scriptStringManager_.findString(4, "lethe_2012_summer_event_mail", (bool*)0);
    }
    std::strncpy(titleBuf, title, 0x14);
    std::strncpy(mailBuf, mail, 0xff);

    ENUM_SERVER_GROUP group = m_user->GetServerGroup();
    unsigned int characNo = m_user->get_charac_no(-1);
    int mailRet = WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        titleBuf, item, 0, characNo, mailBuf, (int)std::strlen(mailBuf),
        0xc, group, false, false);
    if (mailRet == 0)
    {
        EventRewardItemArad data = make_data(eventCode);
        arad::SigSaveRewardEventItem sig;
        sig.set(eventCode, data.m_accId, data.m_characNo, itemIndex, count,
                param);
        Stream* stream =
            GlobalData::s_stream_pool->Acquire("localjapan/Arad_EventPeriodDataManager.cpp",
                                               0x6d);
        CStreamGuard guard(stream, true);
        DISPATCHER::make_internal_stream_jpn(guard,
                                             (ENUM_INTERNALPACKET_JPN)0x27,
                                             (int)m_user->GetUID());
        guard.put_binary(&sig, 0x16);
        MsgQueueMgr::QUEUE_IDX idx = MsgQueueMgr::QUEUE_IDX(2);
        GlobalData::s_msgq_mgr->put(idx, guard);
    }
    else
    {
        LogManager::logFormat(
            1, "localjapan/Arad_EventPeriodDataManager.cpp",
            __PRETTY_FUNCTION__,
            0x66,
            "[RewardLethe] send mail fail. (charac:%u, item:%u, cnt:%d)",
            characNo, (unsigned int)itemIndex, (int)count);
        return false;
    }
    return true;
}

}  // namespace ARAD

// ============================================================================
// ARAD::Arad_EventPeriodDataManager::eventReward<F> 模板主函数 +
// 两个显式实例化（ORIG W 0x0819abc8 / 0x0819adb6，各 493B 逐指令等价转写）。
// ============================================================================
template <typename F>
bool ARAD::Arad_EventPeriodDataManager::eventReward(ENUM_REPEAT_EVENT_CODE eventCode,
                                                    unsigned int time,
                                                    CUser* user)
{
    // 入口门禁：user 空 或 当前角色编号为 0 → 直接失败返 false（ORIG 0xabd0..0xac07）。
    if (user == 0)
        return false;
    if (user->get_charac_no(-1) == 0)
        return false;

    F fn(user);                                        // ORIG call C1(user)
    EventRewardItemArad data = fn.make_data((int)eventCode);
    if (isRewarded(data))                              // ORIG 0x081958b6（按值传参）
        return false;

    SearchEventPeriod pred(eventCode);                 // ORIG SearchEventPeriodC1
    typename std::vector<SCRIPT::SRewardPeriodData>::iterator it =
        std::find_if(m_rewardPeriodList.begin(), m_rewardPeriodList.end(), pred);
    if (it == m_rewardPeriodList.end())
        return false;

    // ORIG 0x819ad0b：elem+0xc(begin) <= time && elem+0x10(end) > time
    if (!(it->m_beginTime <= time && it->m_endTime > time))
        return false;

    // ORIG 0x819ad77：fn(time, elem+4(itemIndex), sw(elem+8)(count), elem+10(endTime))
    if (!fn((int)eventCode, it->m_field4, it->m_field8, it->m_endTime))
        return false;

    addUserRewardData(user, data);                     // ORIG 0x08195aa4（引用重载）
    return true;
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
bool ARAD::SCRIPT::AradJumping_Script::getCurRewardItem(int job, int growValue,
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
