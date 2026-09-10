int Dispatcher_Avatar_Disjoint_Random_Reward::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stAvatarDisjointRandomReward* local_18 = (stAvatarDisjointRandomReward*)&param;
    MSG_AVATAR_DISJOINT_RANDOM_REWARD* local_1c = (MSG_AVATAR_DISJOINT_RANDOM_REWARD*)&msg;
    Inven_Item local_59;                 // -0x55：GetInvenSlot 输出
    char cVar1;

    if (user->get_state() != 3)
    {
        local_18->m_4 = 7;
        return 0;
    }
    EventClassify::CEventScriptMng* pCVar3 =
        (EventClassify::CEventScriptMng*)G_CDataManager()->get_event_script_mng();
    EventClassify::CAvatarDisjointRandomReward* local_14 =
        (EventClassify::CAvatarDisjointRandomReward*)pCVar3->get_event_entity(0x197);
    if (local_14 == 0)
    {
        local_18->m_4 = 0xb2;
        return 0;
    }
    cVar1 = (char)local_14->is_eventing();   // vtable +0x08 槽
    if (cVar1 != 1)
    {
        local_18->m_4 = 0xb2;
        return 0;
    }
    CInventory* pCVar7 = (CInventory*)user->getCurCharacInvenR();
    local_59 = pCVar7->GetInvenSlot(2, (unsigned int)local_1c->m_slotE);
    if (local_59.m_addInfo != local_1c->m_itemIdx10)
    {
        local_18->m_4 = 0x11;
        return 0;
    }
    int local_57 = local_59.m_addInfo;
    CItem* local_c = G_CDataManager()->find_item(local_57);
    if (local_c == 0)
    {
        local_18->m_4 = 0x11;
        return 0;
    }
    unsigned int uVar4 =
        (unsigned int)local_14->checkAvatarDisjointError(user, (unsigned short)local_1c->m_slotE,
                                                         local_59, local_c);
    CInventory* inven = (CInventory*)user->getCurCharacInvenR();
    local_18->m_4 = (int)uVar4;
    if (local_18->m_4 != 0)
    {
        return 0;
    }
    uVar4 = (unsigned int)local_14->deleteAvatarItem(user, (unsigned short)local_1c->m_slotE,
                                                     local_59, local_c);
    local_18->m_4 = (int)uVar4;
    if (local_18->m_4 != 0)
    {
        return 0;
    }
    uVar4 = (unsigned int)local_14->getRewardItem(local_c, local_18->m_rewards);
    local_18->m_4 = (int)uVar4;
    if (local_18->m_4 != 0)
    {
        return 0;
    }
    local_14->SendCreateEmblemStatistic(local_18->m_rewards);
    uVar4 = (unsigned int)local_14->sendRandomItemReward(user, local_18->m_rewards);
    local_18->m_4 = (int)uVar4;
    if (local_18->m_4 != 0)
    {
        return 0;
    }
    local_18->m_8 = (unsigned int)local_1c->m_slotE;
    return 0;
