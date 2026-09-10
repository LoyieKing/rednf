bool CHARAC_LOAD_MERCENARY::Reward(CUser* user,
                                   competition_reward_result& result)
{
    if (user == 0)
        return 0;
    if (GetCompetitionState() == 0)
        return 0;

    static Inven_Item emptyItem;
    static std::string mailTitle(
        g_scriptStringManager_.findString(4, "game_server_msg_225", 0),
        std::allocator<char>());
    static std::string serverMessage;
    static char strMessage[0x100];

    int gold = 0;
    bool bItemGot = false;
    // [b28d-solo-CUserMissingA2②] ORIG 0x081b3dca `mov BYTE PTR [ebp-0x3a],0x0`
    // 与 gold/bItemGot 预清同处（try 之前）⇒ isFinal 是函数头具名局部（P212），
    // 非块内声明。
    bool isFinal = false;
    try
    {
        gold = GetRewardGold(user, result);
        bItemGot = GetRewardItem(user, result, emptyItem, serverMessage);
        if (bItemGot != true)
            serverMessage =
                MercenaryHardCodingSettingValue::MERCENARY_BASE_POSTAL_MESSAGE;
    }
    catch (...)
    {
        m_field28 = 0;
        m_field2c = 0;
        m_field30 = 0xff;
        m_field31 = 0xff;
        alter();
        cMyTrace(__PRETTY_FUNCTION__, 0x1ea, 5)(
            "mercenary Line:%d exception\n", 0x1ea);
        return 1;
    }

    if (gold < 1)
    {
        int term = GetCompetitionPeriodTerm(&isFinal);
        user->m_historyLog.MercenaryReturn(m_characNo, m_field28, m_field2c,
                                           term, gold, 0);
        m_field28 = 0;
        m_field2c = 0;
        m_field30 = 0xff;
        m_field31 = 0xff;
        alter();
        return 1;
    }

    const char* msgText = g_scriptStringManager_.findString(
        4, serverMessage.c_str(), 0);
    strncpy(strMessage, msgText, 0xff);
    size_t msgLen = strlen(strMessage);
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        mailTitle.c_str(), emptyItem, gold, m_characNo, strMessage, msgLen,
        0x1e, user->GetServerGroup(), false, false);

    // ORIG 0x081b3f?? 无预清（`movzbl -0x3a,%eax; test` 直读，GetCompetitionPeriodTerm
    // 命中路径必写）⇒ 不初始化（P198① 反向：ORIG 无 movb $0）。
    bool isFinal2;
    int term2 = GetCompetitionPeriodTerm(&isFinal2);
    user->m_historyLog.MercenaryReturn(m_characNo, m_field28, m_field2c,
                                       term2, gold, 0);

    char status = isFinal2 ? 2 : 1;
    // ORIG 0x081b3fc?：emptyItem.m_addInfo 两处各自独立读取（P48 无 CSE，
    // `mov 0x9407ec2,%eax` ×2），不设 itemIdx 具名局部。
    CItem* pItem = G_CDataManager()->find_item(emptyItem.m_addInfo);
    int rarity = -1;
    if (pItem != 0)
        rarity = pItem->get_rarity();
    DB_MercenaryLogInfo::makeRequest(
        user->get_acc_id(), m_characNo, m_field28,
        GlobalData::s_systemTime_.getCurSec(), status, m_field31, m_field30,
        gold, emptyItem.m_addInfo, rarity);

    m_field28 = 0;
    m_field2c = 0;
    m_field30 = 0xff;
    m_field31 = 0xff;
    alter();
    return 1;
