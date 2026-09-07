// df_game_r 还原 —— CPvPLiveEventData::IncreasePlayCount（ORIG 0x826921c）。
// 其余方法在本 TU 复用真实成员布局；loadData 只恢复缓冲区首 dword。
#include "CPvPLiveEventData.h"
#include "GlobalData.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"

#include <string.h>
#include <string>

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CDataManager.h"
#include "CItemList.h"
#include "Inven_Item.h"
#include "ServerParameterScript.h"
#include "RDARScriptStringManager.h"
#include "CGameManager.h"      // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail

extern CDataManager* G_CDataManager();

void CPvPLiveEventData::IncreasePlayCount(CUser* user)
{
    // ORIG 0x826921c：计数达阈值后按奖励表发系统邮件。
    if (user->getCurCharacR() == 0)
        return;

    alter();                       // charac_expand::CData::alter
    m_playCount += 1;              // +0x08

    CDataManager* dm = G_CDataManager();
    if (m_playCount != dm->m_serverParameter.GetPvPLiveEventPlayCount())
        return;

    std::vector<std::pair<int, int> >& reward =
        dm->m_serverParameter.GetPvPLiveEventReward();
    for (std::vector<std::pair<int, int> >::iterator it = reward.begin();
         it != reward.end(); ++it)
    {
        int itemIdx = it->first;
        int addInfo = it->second;
        if (dm->find_item(itemIdx) == 0)
            continue;

        Inven_Item item;
        dm->m_itemList->create_item(ENUM_ITEM_CREATE_TYPE_0, item, addInfo);
        if (itemIdx == 0 && item.get_add_info() == 0)
            continue;

        const char* titleSrc =
            g_scriptStringManager_.findString(4, "game_server_msg_140", 0);
        const char* bodySrc =
            g_scriptStringManager_.findString(4, "game_server_msg_141", 0);
        char title[0x15];
        char body[0x100];
        strncpy(title, titleSrc, 0x14);
        title[0x14] = '\0';
        strncpy(body, bodySrc, 0xff);
        body[0xff] = '\0';

        ENUM_SERVER_GROUP group = user->GetServerGroup();
        unsigned int characNo =
            (unsigned int)user->getCurCharacNo();
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            title, item, 0, characNo, body, (int)strlen(body), 0, group, false, false);
        item.reset();
    }
}


CPvPLiveEventData::CPvPLiveEventData()
{
}

bool CPvPLiveEventData::loadData(CUser* /*user*/, char* buffer)
{
    m_playCount = *reinterpret_cast<int*>(buffer);
    return true;
}
void CPvPLiveEventData::_reset()
{
    m_playCount = 0;
}

void CPvPLiveEventData::ResetDailyMidnight()
{
    m_playCount = 0;
}

void CPvPLiveEventData::getData(char* pData) const
{
    // ORIG 0x08269206：函数体为空（仅 push/pop/ret）。
    // 播放计数不在此序列化，由 _saveData 走 CStreamGuard 落库。
}

// [b23判词·N-GUARD] P167 调用点差集 CLEAR（19/19）：ORIG 每处 operator<< 前调
// CStreamGuard::operator*()（deEv）并复用返回 %eax 作 operator<< 的 this。Game/CStreamGuard.h
// 的 operator* 返回 CStreamGuard*（指针），本工程无法 `*guard<<x` 链式复用 eax ⇒ 用显式
// `guard.operator*(); guard<<x;` 弃值形（Main 统一口径）。残余 gap+3 = 弃值调用无法把
// operator* 结果留 %eax 直传 operator<<（多 %ebx 缓存/重装载），跨工具链 guard ABI 形不可复现，语义等价。
bool CPvPLiveEventData::_saveData(CUser* user)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("PvPLiveEventData.cpp", 0x1e), true);
    // ORIG 每处先调 CStreamGuard::operator*()（deEv）再 operator<<。
    // Game/CStreamGuard.h 的 operator* 返回 CStreamGuard*（this 指针）⇒ `*guard << x`
    // 形在指针上左移非法（本地 Stream& 版 guard 类的 `*guard<<` 不可移植到此 TU）；
    // 等价发射形=显式调用弃值 + guard 成员 <<（Main 修链，同 GmMacro 案）。
    guard.operator*();
    guard << 0x1e1;
    guard.operator*();
    guard << user->GetUID();   // P197② 语句表达式实测不升格 ebx（4.4 恒直存 outgoing 槽）
    guard.operator*();
    guard << (char)user->GetServerGroup();   // ORIG lsEc（movsbl 符号扩展，写 1 字节）
    guard.operator*();
    guard << user->getCurCharacNo();
    guard.operator*();
    guard << m_playCount;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}
