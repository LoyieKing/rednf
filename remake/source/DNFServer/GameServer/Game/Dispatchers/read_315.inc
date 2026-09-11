// ==== Dispatcher_MailBox_Send::read (ORIG 0x081cc3da) ====
int Dispatcher_MailBox_Send::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081cc3da：依次解析 10 个字段；失败分支各自 LineFunc 并返回其返回值。
    // 收件人名/金币/空间/槽位/物品 id/附加字段 共用 0x2315（ORIG 同一跳转目标），
    // 标题长 0x231b、文本 0x231e、公会邮键 0x2321 独立。
    // 字段经 MSG_MAILBOX_SEND 成员直访（O0 下 inline helper 不内联会多出 call，
    // 故本函数不走 mbXxx helper）。
    MSG_MAILBOX_SEND& m = reinterpret_cast<MSG_MAILBOX_SEND&>(msg);
    if (!packet.get_int(m.m_nameLenD))                                // +0x0d 收件人名长
        return LineFunc(0x2309, __PRETTY_FUNCTION__, 0, 0);

    if (!packet.get_str(m.m_name11, 0x1d, m.m_nameLenD) ||           // +0x11 收件人名
        !packet.get_int(m.m_gold2e) ||                               // +0x2e 金币
        !packet.get_byte(m.m_space36) ||                             // +0x36 物品空间
        !packet.get_short(m.m_slot37) ||                             // +0x37 槽位
        !packet.get_int(m.m_itemId39) ||                             // +0x39 物品 id
        !packet.get_int(m.m_extra3d))                                // +0x3d 附加字段
        return LineFunc(0x2315, __PRETTY_FUNCTION__, 0, 0);

    m.m_titleLen41 = 0;                                              // +0x41 标题长清零
    m.m_guildKey32 = 0;                                              // +0x32 公会邮键清零
    memset(m.m_text45, 0, 0x100);                                    // +0x45 文本区清零

    if (!packet.get_int(m.m_titleLen41))                             // +0x41 标题长
        return LineFunc(0x231b, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(m.m_text45, 0x100, m.m_titleLen41))          // +0x45 邮件文本
        return LineFunc(0x231e, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(m.m_guildKey32))                             // +0x32 公会邮键
        return LineFunc(0x2321, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}