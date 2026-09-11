// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 35（ORIG 201 / ours 198），残差=P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
void DisPatcher_SelectDungeon::send(CUser* user, ParamBase& param)

{
    stSelectDungeonParam* local_14 = &static_cast<stSelectDungeonParam&>(param);
    // [b28d] P223 多岛细则：ORIG 尾部有三个连续 nop 中继岛（0x081c9164/167/16a），
    // 三条守卫 je 各落一个 ⇒ 用三条不同源的负条件 goto 到连续尾空标签复现
    // （标签序=守卫序；4.4 单源裸 if 恒 jump-thread 成同一落点，gap=6）。
    if (local_14->m_4 == 0x7fffffff)
    {
        goto L_m4;
    }
    if (local_14->m_4 != 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x10),
                                 static_cast<unsigned char>(local_14->m_4));
    }
    if (local_14->m_c == 0)
    {
        if (local_14->m_8 == 0)
        {
            goto L_party;
        }
    }
    {
        CParty* local_10 = static_cast<CParty*>(user->GetParty());
        if (local_10 == 0)
        {
            goto L_cparty;
        }
        if (local_14->m_8 == local_14->m_c)
        {
            PacketGuard local_20;
            local_20.put_header(1, 0x10);
            local_20.put_byte(0);
            local_20.put_byte(local_14->m_8);
            if (local_14->m_8 == 0x16)
            {
                local_20.put_byte(local_14->m_14);
            }
            local_20.finalize(true);
            local_10->send_to_party(local_20);
        }
        // [b28d] ORIG 0x081c8fe9/0x081c9097 块布局：`m_8!=m_c` 时先判
        // `m_8!=0`（fall-through 到 user->Send 体），`m_8==0` 才跳去
        // `m_c!=0` 的 send_to_party 体 ⇒ 原码是 `else if(m_8!=0)` 在前、
        // `else if(m_c!=0)` 在后（P272 块序交换）。
        else if (local_14->m_8 != 0)
        {
            PacketGuard local_2c;
            local_2c.put_header(1, 0x10);
            local_2c.put_byte(0);
            local_2c.put_byte(local_14->m_8);
            local_2c.finalize(true);
            user->Send(local_2c);
        }
        else if (local_14->m_c != 0)
        {
            PacketGuard local_38;
            local_38.put_header(1, 0x10);
            local_38.put_byte(0);
            local_38.put_byte(local_14->m_8);
            if (local_14->m_c == 0xad)
            {
                local_38.put_byte(local_14->m_14);
            }
            local_38.finalize(true);
            local_10->send_to_party(local_38);
        }
    }
L_m4:;
L_party:;
L_cparty:;
}
