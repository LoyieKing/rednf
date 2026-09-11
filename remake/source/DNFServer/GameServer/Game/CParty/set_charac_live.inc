// ============================================================================
// CParty::set_charac_live —— ORIG 0x085b29bc（0x195 字节）
//   报告：docs/class_func_reports/CParty/set_charac_live.md
//   找到 user 槽位后按 state 写 +0x380(live)/+0x384(enterMap)/+0x388 标志；
//   state==1 时 m_member[i].m_field14(+0x8c) 计数 +1；随后发包 0x20 广播。
// ============================================================================
void CParty::set_charac_live(CUser* user, ENUM_USER_DUNGEON_STATE state)
{
    // b28d-solo-CParty @0x85b29bc：ORIG 为 for（头部 jmp 到 +0x17f 尾测 setle）
    // + 两条独立守卫（P133）+ 处理段尾 return；while(true)+break 形产直跳无 setle。
    // [G7 试形A] 双 continue 使 4.4 在循环增量前多发中继岛（nop+jmp+nop，我方 +0x183）
    // ⇒ ORIG 0x085b2b36/0x085b2b37 仅 nop+addl。按 P296 把第二守卫改正向包裹。
    for (int slot = 0; slot <= 3; ++slot)
    {
        if (!_checkValidUser(slot))                    // ORIG xor $1;test;jne
            continue;
        if (m_member[slot].m_pUser == user)            // ORIG cmp 0xc(%ebp);jne
        {
            // b28d-solo-CParty：ORIG 0x085b2a0d 先 `mov 0x10(%ebp),%eax; test; je <尾 0 岛>`
            // ⇒ 负条件 if/else（P222/P257：零分支落尾、state 只载一次）。
            // [P251/P286 试形记录（已回滚）] `switch (state){case 0:…}` 实测产 126 条
            // （比 if 形多 1）且首比较仍非 `mov 0x10(%ebp),%eax; test` 寄存器链
            // ⇒ 4.1「一次载入 + 三次使用」形不可复现，判 N18b 族。
            if (state != ENUM_USER_DUNGEON_STATE_0)
            {
                if (state >= ENUM_USER_DUNGEON_STATE_0 && state < (ENUM_USER_DUNGEON_STATE)3)
                {
                    m_battleData.m_arr54[slot] = 1;            // +0x380
                    m_battleData.m_arr58[slot] = 1;        // +0x384
                    m_battleData.m_arr5c[slot] = 1;            // BattleData 第三标志组首字节
                    if (state == (ENUM_USER_DUNGEON_STATE)1)
                        m_member[slot].m_field14 =
                            m_member[slot].m_field14 + 1;   // ORIG 复用 edx 单次载 slot（P38）
                }
            }
            else
            {
                m_battleData.m_arr54[slot] = 0;            // +0x380
            }

            PacketGuard packet;
            packet.put_header(0, 0x20);
            packet.put_short(user->get_unique_id() & 0xffff);
            packet.put_byte(state);
            packet.finalize(true);
            send_to_party(packet);
            return;
        }
    }

}