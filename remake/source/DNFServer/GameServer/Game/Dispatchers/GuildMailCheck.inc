// ==== Dispatcher_MailBox_Send::GuildMailCheck (ORIG 0x081cc8c2) ====
int Dispatcher_MailBox_Send::GuildMailCheck(CUser* user, MSG_BASE& msg)
{
    if (user->get_charac_guildkey() == 0)   // ORIG：取角色公会键（非 msg 字段）
        return 0x22;

    if (!user->IsSubGuildMaster() && !user->IsGuildMaster())    // 仅会长/副会长可发
        return 0x24;

    // ORIG 0x081cc91e-929: fee → -0xc 槽；0x081cc932: getCurCharacInvenR 结果
    // 不落栈（mov %eax,(%esp) 直传 get_money，链式内联）。
    const int fee = G_CDataManager()->m_guildParameter.m_mail;  // +0xa624 公会邮件费（m_guildParameter.m_mail）
    if (user->getCurCharacInvenR()->get_money() < fee)
        return 0x23;
    return 0;
}