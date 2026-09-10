unsigned int Dispatcher_MailBox_Send::check_error(CUser* user, MSG_BASE& msg)
{
    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
        return -1;

    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, RestrictCategory::Enum_1, 4))
        return 0xd1;

    const int secuErr =
        GlobalData::s_pSecuProtectionField->Check(user, static_cast<SECURITY_PROTCTION>(0x3f));
    if (secuErr != 0)
        return secuErr;

    const int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)static_cast<int>(mbSpace(msg)));
    const int slot = static_cast<int>(mbSlot(msg));
    if (user->CheckItemLock(invenType, slot))
        return 0xd5;

    {
        bool movable = true;
        CheckItemTradeLimitCountToMoveByUser(*user, invenType, slot, movable);
        if (!movable)
            return 0x72;
    }

    {
        const std::string title(mbText(msg));                   // +0x45 文本入 std::string
        if (CSyncSlangFilter::GetInstance()->HasSlang(title))
            return 0x9f;
    }

    if (strlen(mbText(msg)) != static_cast<size_t>(mbTitleLen(msg)))
        return -3;                                              // 标题长与实际串长不符

    if (mbItemId(msg) == 0 &&
        mbGold(msg) == 0 &&
        mbText(msg)[0] == '\0')
        return 0x16;                                            // 纯空邮件

    if (mbGuildKey(msg) != 0) {
        const int guildErr = GuildMailCheck(user, msg);
        if (guildErr != 0)
            return guildErr;
    }

    if (user->GetReliablePerson() == 1)
        return 0;
    if (!user->IsAvailableCurCharacTradeGoldDaily(mbGold(msg)))
        return 0x46;                                            // 当日交易金币额度不足
    return 0;
