int Inter_ChagneUserHandicap::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_ChagneUserHandicapPktView* pPkt = reinterpret_cast<Inter_ChagneUserHandicapPktView*>(data);
    CUser* target = G_CGameManager()->GetUserByAccId(
        pPkt->m_a);
    /* [b28b P0/N96 清绑复测] ORIG 0x084d2745-084d276b 守卫 =「target==0 短路 je →
     * get_state; cmp $1; jg →mov$0；汇合 test %al,%al → je 早退」的 OR 直判形（bool 落
     * %al 寄存器、无独立槽）。P168a① 裸形复测：直接 `if (target == 0 || state <= 1) return 0;`
     * 即由 4.4 复现 mov$1/mov$0 + test %al 汇聚，与 ORIG 全等——存量 register bool asm("al")
     * 绑定违反 P0，已删；无需约束形。 */
    if (target == 0 || target->get_state() <= 1)
        return 0;
    if (pPkt->m_e == 1)
    {
        target->setStdDropRate(pPkt->m_12);
    }
    return 0;
}