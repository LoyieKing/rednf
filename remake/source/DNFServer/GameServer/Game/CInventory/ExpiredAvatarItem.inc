void CInventory::ExpiredAvatarItem(PacketGuard* packet)
{
    int now = OS_API::GetDateTimeTick();
    // b28c-L2: ORIG 0x8509501/0x8509524 两 je 目标差 1 字节（cond1 落 nop 岛、
    // cond2/3 落 addl）⇒ 原码是「外层 m_addInfo!=0 + 内层短路」双层嵌套，
    // 非单条 && 短路链（4.4 对合并链三 je 同目标）。
    // 续（b28c-L2）：ORIG body 尾 `jmp <addl>` + nop 岛 ⇒ 负条件 goto 尾标签形
    // （探针 l2_probe6 Z 命中，4.4 产 je/jge 尾标签 + body 尾 jmp）。
    for (int i = 0; i <= 9; i++)
    {
        if (m_invenItem[i].m_addInfo == 0) goto next_inven;
        if (m_invenItem[i].m_addInfo2 == 0 || m_invenItem[i].m_addInfo2 >= now) goto next_inven;
        SendAvatarEvent(0x33, m_pParent->get_charac_no(-1), i, 0, 0);
        m_invenItem[i].reset();
    next_inven:;
    }
    for (int i = 0; i < 0x69; i++)
    {
        if (m_pAvatarSlot[i].m_addInfo == 0) goto next_avatar;
        if (m_pAvatarSlot[i].m_addInfo2 == 0 || m_pAvatarSlot[i].m_addInfo2 >= now) goto next_avatar;
        SendAvatarEvent(0x33, m_pParent->get_charac_no(-1), i + 10, 0, 0);
        m_pAvatarSlot[i].reset();
    next_avatar:;
    }
