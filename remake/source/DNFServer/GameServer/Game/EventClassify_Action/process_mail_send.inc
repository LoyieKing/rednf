// ============================================================================
// process_mail_send（ORIG 0x0810da1c）
// citem 为空回 0x11；avatar 物品不入邮件（仅清局部缓冲），其余走 DB 邮件。
// [b28d-solo-EventClassify_Action] ORIG 0x0810da1c，173=173 AE。
//   分支极性 `if (IsAvatarItem()) {avatar 死缓冲} else {DB 邮件}` 与
//   title[256]={0}/sender[30]={0} 预清、findString(4,toTString)=strncpy、
//   ReqDBSendNewSystemMail 九参装载逐条对齐（旧 N82 判词系旧 .o 假象，已撤销）。
// ============================================================================
int CEventActionMng::process_mail_send(CUser* user, Action_SendMail& mail,
                                       Inven_Item& item, CItem const* citem)
{
    if (citem == 0)
        return 0x11;

    char title[256] = {0};
    char sender[30] = {0};
    std::strncpy(sender, g_scriptStringManager_.findString(
                     4, toTString(mail.m_field30), 0), 0x1d);
    std::strncpy(title, g_scriptStringManager_.findString(
                     4, toTString(mail.m_field34), 0), 0xff);

    // ORIG 分支极性：`test %al,%al; je <DB-mail>` —— avatar（IsAvatarItem 非 0）为
    // if-body，DB-mail 为 else；反写匹配 ORIG 块序。
    if (citem->IsAvatarItem())
    {
        // avatar 分支：ORIG 仅清零 0x18 字节局部缓冲并置 0 一个 int
        // 后返回 0（无 DB 请求）。mailPad/dummy 均为忠实还原的死存储。
        char mailPad[24] = {0};
        int dummy = 0;
    }
    else
    {
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            sender, item,
            (unsigned int)mail.m_gold,
            (unsigned int)user->getCurCharacNo(),
            title, (int)std::strlen(title),
            (unsigned int)(signed char)mail.m_coin,
            user->GetServerGroup(), false, false);
    }
    return 0;
}