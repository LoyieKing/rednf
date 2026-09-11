// ==== Dispatcher_MailBox_Send::check_error (ORIG 0x081cc636) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N89] ORIG 0x081cc636 orig/ours=196/226 ndiff-blocks=25
//   ORIG 段内 std::/__gnu_cxx out-of-line 内联实例 call 7 个，4.4 libstdc++ 全内联不可复现
//   首差: mov    &_ZN10GlobalData22s_pSecuProtectionFieldE,%eax | mov    %eax,-0x14(%ebp) | cmpl   $0x0,-0x14(%ebp)
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

    // [G1-Dispatchers] 0x081cc6d6..：ORIG 先 `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)`（msg 具名
    // 指针局部，P0/P244），随后全部字段**直读**（0x37/0x36/0x45/0x41/0x39/0x2e/0x32），无任何
    // helper call ⇒ 原码用 MSG_MAILBOX_SEND* 具名局部访问真实字段；本文件原有 mbX() 内联函
    // 数在 -O0 下会发射 `_ZN12_GLOBAL__N_1…` 调用（ORIG 无，p167 EXTRA 6 项）。
    MSG_MAILBOX_SEND* req = reinterpret_cast<MSG_MAILBOX_SEND*>(&msg);
    if (user->CheckItemLock(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)req->m_space36),
            (int)req->m_slot37))
        return 0xd5;

    {
        bool movable = true;
        // P48：ORIG 0x081cc71d..0x081cc74e 重发 slot/space 装载与 GetInvenTypeFromItemSpace
        // call（第二次，不缓存首值）⇒ 此处必须重算而不是复用上面两个实参。
        CheckItemTradeLimitCountToMoveByUser(
            *user, GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)req->m_space36),
            (int)req->m_slot37, movable);
        if (!movable)
            return 0x72;
    }

    {
        const std::string title(req->m_text45);                 // +0x45 文本入 std::string
        if (CSyncSlangFilter::GetInstance()->HasSlang(title))
            return 0x9f;
    }

    if (strlen(req->m_text45) != static_cast<size_t>(req->m_titleLen41))
        return -3;                                              // 标题长与实际串长不符

    if (req->m_itemId39 == 0 &&
        req->m_gold2e == 0 &&
        req->m_text45[0] == '\0')
        return 0x16;                                            // 纯空邮件

    if (req->m_guildKey32 != 0) {
        const int guildErr = GuildMailCheck(user, msg);
        if (guildErr != 0)
            return guildErr;
    }

    if (user->GetReliablePerson() == 1)
        return 0;
    if (!user->IsAvailableCurCharacTradeGoldDaily(req->m_gold2e))
        return 0x46;                                            // 当日交易金币额度不足
    return 0;
}