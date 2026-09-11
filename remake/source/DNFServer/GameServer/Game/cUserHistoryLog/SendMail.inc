// SendMail：ORIG 0x86847ea。字段已具名（PacketTypes.h MSG_MAILBOX_SEND，
// m_pad 即 MSG_BASE 后的 +0x0d 起原字节区）。条件为 mail 非空 且 收件人名非空。
void cUserHistoryLog::SendMail(MSG_MAILBOX_SEND* mail, unsigned int a)
{
    // b28d-65：ORIG 0x086847ea 两条独立早退（je 分落两个 jmp/nop 中继，P63）：
    // 第二守卫 `add $0x11; test %eax,%eax` 是数组退化比较（恒真、无 movzbl）
    // ⇒ 原码写 `mail->m_name11 == 0`（P171 照抄原码恒真形）；nstr/f2e/f36/f37
    // 无槽往返 ⇒ 内联实参（P0），mailBody/f3d/f39 有槽 ⇒ 具名局部。
    if (mail == 0)
        return;
    if (mail->m_name11 == 0)
        return;
    m_user->LogHistory("MailS,%s,%d,%d,%d,%d,%d,\"%s\",%s",
                       mail->m_name11, mail->m_gold2e, mail->m_space36,
                       mail->m_slot37, mail->m_itemId39, mail->m_extra3d,
                       mail->m_text45, NumberToString(a, 0));
}