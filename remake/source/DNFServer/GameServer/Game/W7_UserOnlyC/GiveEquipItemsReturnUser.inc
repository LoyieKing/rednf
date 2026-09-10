void CUser::GiveEquipItemsReturnUser()
{
    // P0/P48：ORIG growType/level/growType2/job 均无槽（%ebx/%eax 直传）⇒ 内联实参；
    // key 有具名槽 -0x24。getCurCharFirstGrowType 每次使用均重调（两遍）。
    const int key = getReturnUserLevelKey(get_charac_level(),
                                          getCurCharFirstGrowType());

    LevelUpRewardItemTableKey tableKey(key, get_charac_job(),
                                       getCurCharFirstGrowType());
    LevelUpRewardItemData data;
    // P168a②：ORIG `call; xor $1; test %al; jne` ⇒ bool callee 的否定形
    // `if (!get_data(...))`（4.4 直产 xor 段）。
    if (!G_CDataManager()->m_returnUserReward.m_levelUp.get_data(tableKey,
                                                                 data))
    {
        return;       // P285：外层空 else 保留 ORIG 的 jmp+nop 岛
    }
    else
    {
        Inven_Item item;
        CItem* base = G_CDataManager()->find_item(data.m_field0);
        if (base != 0)
        {
            item.m_addInfo = data.m_field0;
            base->make_item(item);
            item.set_add_info(data.m_field4);

            const char* msgBody =
                g_scriptStringManager_.findString(4, "return_user_msg_2", 0);
            // P0/P52：group/msgLen/characNo/sender 无具名槽 ⇒ 内联实参。
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                g_scriptStringManager_.findString(4, "return_user_msg_title",
                                                  0),
                item, 0u, static_cast<unsigned int>(getCurCharacNo()),
                msgBody, static_cast<int>(strlen(msgBody)), 7u,
                GetServerGroup(), false, false);
        }
        else
        {
            return;   // P285：内层空 else 保留 ORIG 的 nop 占位岛
        }
    }
