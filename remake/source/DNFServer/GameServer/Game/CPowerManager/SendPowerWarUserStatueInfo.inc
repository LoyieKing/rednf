// [b28d-solo-CPowerManager] IDENTICAL(237=237, ndiff 0) | ORIG 0x0847f200 | 本轮修复：
// P310（循环变量 i 由 unsigned 改 int + 显式 `(unsigned int)i < 3`）复现 ORIG 尾测
// 两段物化 `mov -0x14(%ebp),%eax; cmp $0x2,%eax; setbe; test; jne`，旧「N18b 尾测
// 1 条」判词作废。此前：len 具名缓存消 EXTRA +2 strlen（P167 22/22 CLEAR）、
// P47 正极性 if/else、P212 全头部声明（槽位全等 pkt-0x28/count-0x1c/len-0x18/
// i-0x14/i2-0x10/j-0xc）、P171 死预清 len=0。
void CPowerManager::SendPowerWarUserStatueInfo(CUser* user)
{
    // ORIG 0x0847f200：组 SEND 雕像信息包（0xb6），含各条目姓名/职业/等级/
    // 成长/公会名/公会id/装备，user==0 全服广播，否则单发。
    // [b28d-solo] P212：ORIG 槽深序 count(-0x1c) < len(-0x18) < i(-0x14) <
    // i2(-0x10) < j(-0xc)；4.4 块内局部按首次使用序分配 ⇒ 全部提至函数头声明
    // （声明序 = ORIG 槽深序），块内仅赋值。
    PacketGuard pkt;
    int count;
    int len;
    int i;
    int i2;
    int j;
    pkt.put_header(0, 0xb6);
    count = 0;
    for (i = 0; (unsigned int)i < 3; ++i)
        if (m_rankings[i].m_characNo != 0)
            ++count;
    pkt.put_byte((int)(char)count);
    len = 0;   // [b28d-solo] P171：ORIG 0x0847f2xx `movl $0x0,-0x18` 死预清（照抄）
    for (i2 = 0; i2 < count; ++i2)
    {
        // [b28d-solo] P0/P102：ORIG 用单一具名 len 槽（-0x18）缓存 strlen 结果，
        // put_int/put_str 共用 ⇒ 每处仅 1 次 strlen（原内联两次 = EXTRA +2 strlen）。
        len = (int)strlen(m_rankings[i2].m_name);
        pkt.put_int(len);
        pkt.put_str(m_rankings[i2].m_name, len);
        pkt.put_byte((int)(char)m_rankings[i2].m_job);
        pkt.put_byte((int)(char)m_rankings[i2].m_level);
        pkt.put_byte((int)(char)m_rankings[i2].m_grow);
        len = (int)strlen(m_rankings[i2].m_guildName);
        pkt.put_int(len);
        pkt.put_str(m_rankings[i2].m_guildName, len);
        pkt.put_int(m_rankings[i2].m_guildId);
        for (j = 0; j < 0xb; ++j)
        {
            if (m_rankings[i2].m_equipment[j] == 0 || j == 9)
                pkt.put_int(-1);
            else
                pkt.put_int(m_rankings[i2].m_equipment[j]);
        }
    }
    pkt.finalize(true);
    // [b28d-solo] P47/P222：ORIG `cmp; je <send_all 段>`（Send 直落）⇒ 正极性
    // `if (user != 0) Send; else send_all;`（原 ==0 首臂产反向 jne 块布局）。
    if (user != 0)
        user->Send(pkt);
    else
        G_GameWorld()->send_all(pkt);
}