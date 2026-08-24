// df_game_r 还原 —— CPvPLiveEventData::IncreasePlayCount（ORIG 0x826921c）。
// 其余方法（ResetDailyMidnight/_reset/loadData/_saveData）仅声明不定义。
#include "CPvPLiveEventData.h"

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
