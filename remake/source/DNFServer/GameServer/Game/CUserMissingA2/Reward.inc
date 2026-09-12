// ----------------------------------------------------------------------------
// 23. ORIG T 0x081b3b68 sz 0x5a2。结算主流程：GetCompetitionState 非 0 才继续；
// 局部 static emptyItem/mailTitle/serverMessage/strMessage；try{GetRewardGold+
// GetRewardItem}catch 复位+alter+cMyTrace 返回 1；gold<1 走 MercenaryReturn
// 清理；否则发系统邮件（ReqDBSendNewSystemMail）+ 日志 + DB 记录。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N82/N89：ORIG 0x081b3b68；p167 CLEAR（49/49），残差为 STL 实例
// 形态与帧/槽分配，语义闭合。
// [b28d-solo-CUserMissingA2] 修复：`bool isFinal2` 去预清（ORIG 无 movb $0，
// 直读 movzbl -0x3a）+ `emptyItem.m_addInfo` 两处独立读取（P48 无 CSE，消
// itemIdx 具名槽）。p167 CLEAR 49/49；残差为槽序（4.4 首次使用序）+ STL 实例
// 形态 ⇒ N82/N89 封顶。
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
        // [SemA04 2026-09-12] ORIG 0x081b3e62-0x081b3e94：GetCompetitionPeriodTerm 的
        // 返回值**直接**作 MercenaryReturn 第 4 实参（`mov %eax,0x10(%esp)`），无 term
        // 具名槽 ⇒ 按 P0 槽-局部双向对应律内联为实参表达式（原具名局部多 1 槽且
        // 求值提前，时序/帧双错位）。
        user->m_historyLog.MercenaryReturn(m_characNo, m_field28, m_field2c,
                                           GetCompetitionPeriodTerm(&isFinal), gold, 0);
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
    // [SemA04 2026-09-12] ORIG 0x081b3f27-0x081b3f40：GetServerGroup 的 call 在 strlen
    // **之前**、strlen 结果落 -0x5c 临时槽（实参跨调用暂存），无 msgLen 具名局部
    // ⇒ 按 P44/P329（变参/多实参日志的具名中间量必须内联回实参位）内联。
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        mailTitle.c_str(), emptyItem, gold, m_characNo, strMessage,
        strlen(strMessage),
        0x1e, user->GetServerGroup(), false, false);

    // [SemA04 2026-09-12] ORIG 0x081b3f9a-0x081b3ff7：第二处 GetCompetitionPeriodTerm
    // 传的仍是 **-0x3a 槽**（与 gold<1 分支同一 isFinal 变量；该处无预清）且返回值
    // `mov %eax,0x10(%esp)` 直接作第 4 实参 ⇒ 复用函数头 isFinal（P0：无 term2 具名槽）。
    user->m_historyLog.MercenaryReturn(m_characNo, m_field28, m_field2c,
                                       GetCompetitionPeriodTerm(&isFinal), gold, 0);

    char status = isFinal ? 2 : 1;
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
}