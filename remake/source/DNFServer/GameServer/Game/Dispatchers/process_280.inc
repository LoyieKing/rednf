int Dispatcher_Exchange_Random_Item_Reward::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stExchangeRandomItemReward& out = static_cast<stExchangeRandomItemReward&>(param);
    if (user == 0 || user->get_state() != 3)
    {
        out.m_4 = 7;
        return 0;
    }
    const unsigned int eventIdx =
        static_cast<unsigned int>(
            *reinterpret_cast<const unsigned short*>(
                reinterpret_cast<const char*>(&msg) + 0xe));
    EventClassify::CEventScriptMng* mng =
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng());
    EventClassify::CExchangeRandomItemReward* entity =
        static_cast<EventClassify::CExchangeRandomItemReward*>(
            mng->get_event_entity(static_cast<int>(eventIdx)));
    if (entity == 0)
    {
        out.m_4 = 0xb2;
        return 0;
    }
    if (!entity->is_eventing())
    {
        out.m_4 = 0xb2;
        return 0;
    }
    std::vector<std::pair<int, int> > materials;                       // -0x20
    out.m_4 = entity->verifyMaterialItemByRule(
        user,
        reinterpret_cast<MSG_EXCHANGE_RANDOM_ITEM_REWARD&>(msg).m_rewardItems,
        materials);
    if (out.m_4 != 0)
    {
        return 0;
    }
    std::vector<std::pair<int, int> > rewards;                         // -0x2c
    out.m_4 = entity->getRewardItem(rewards);
    if (out.m_4 != 0)
    {
        return 0;
    }
    out.m_4 = entity->deleteUsedMaterialItem(user, materials, out.m_rewards);
    if (out.m_4 != 0)
    {
        return 0;
    }
    out.m_4 = entity->sendRandomItemReward(user, rewards, out.m_items, out.m_9);
    return 0;
