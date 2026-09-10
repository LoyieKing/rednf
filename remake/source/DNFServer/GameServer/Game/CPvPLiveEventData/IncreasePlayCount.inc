void CPvPLiveEventData::IncreasePlayCount(CUser* user)
{
    // ORIG 0x826921c：计数达阈值后按奖励表发系统邮件。
    if (user->getCurCharacR() == 0)
        return;

    alter();                       // charac_expand::CData::alter
    m_playCount += 1;              // +0x08

    if (m_playCount == G_CDataManager()->m_serverParameter.GetPvPLiveEventPlayCount())
    {
        for (std::vector<std::pair<int, int> >::iterator it =
                 G_CDataManager()->m_serverParameter.GetPvPLiveEventReward().begin();
             it != G_CDataManager()->m_serverParameter.GetPvPLiveEventReward().end(); ++it)
        {
            // [b28d-solo-CPvPLiveEventData] ORIG 0x082692c4/0x082692f6 在 G_CDataManager()
            // 调用前把两个值装入 %ebx 跨调用保活（P290：局部 int→unsigned 触发实参升格
            // callee-saved）。声明为 unsigned int 复现该装载序（探针 /tmp/probe_p290.cpp f3/f8）。
            unsigned int itemIdx = (*it).first;
            unsigned int addInfo = (*it).second;
            CItem* found = G_CDataManager()->find_item(({ itemIdx; }));
            if (found != 0)
            {
                Inven_Item item;
                item.m_addInfo = itemIdx;
                G_CDataManager()->m_itemList->create_item(
                    ENUM_ITEM_CREATE_TYPE_0, item, ({ addInfo; }));
                if (item.m_addInfo != 0 || item.get_add_info() != 0)
                {
                    char title[0x15];
                    char body[0x100];
                    strncpy(title,
                            g_scriptStringManager_.findString(4, "game_server_msg_140", 0), 0x14);
                    strncpy(body,
                            g_scriptStringManager_.findString(4, "game_server_msg_141", 0), 0xff);

                    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                        title, item, 0, user->getCurCharacNo(), body, (int)strlen(body), 0,
                        user->GetServerGroup(), false, false);
                    item.reset();
                }
            }
        }
    }
