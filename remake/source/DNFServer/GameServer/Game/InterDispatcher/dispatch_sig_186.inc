int Inter_MonitorPunishUserReq::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_MonitorPunishUserReqPktView* pPkt = reinterpret_cast<Inter_MonitorPunishUserReqPktView*>(data);

    // P227：ORIG 084db2a5/084db2af 在 ctor 前求出 m_b→%esi / m_a→%ebx 并跨 ctor 保活
    //   ⇒ 全表达式临时链（禁具名 trace 局部 + 二次语句；值不进栈槽）。
    cMyTrace(__PRETTY_FUNCTION__, 0x43bc, 0)(
        "Monitor Punish User / m_flag = %d / user_count", (int)pPkt->m_a,
        (unsigned int)pPkt->m_b);

    if (pPkt->m_a == 0)
    {
        int i = 0;
        for (i = 0; i < (int)pPkt->m_b; i = i + 1)     // P191 双初始化：ORIG 084db303/084db30a 两条清零
        {
            // P223：ORIG 084db323 `jbe <nop 岛>` ⇒ 空 then 占位岛用负条件 goto 复现。
            if (pPkt->m_accIds[i] <= 0x63)
                goto next_acc;
            if (G_CGameManager()->isConnectUserByAccId(pPkt->m_accIds[i]) != '\0')
            {
                CUser* target = G_CGameManager()->GetUserByAccId(pPkt->m_accIds[i]);
                target->DisConnSig((DISCONN_SIG)0x2c, true, 0);
                // P227：ORIG 084db3a7 把 NumberToString 结果留 %ebx 跨 ctor ⇒ 临时链内联。
                cMyTrace(__PRETTY_FUNCTION__, 0x43d4, 0)(
                    "System Kick User : %s", NumberToString(pPkt->m_accIds[i], 0));
            }
        next_acc:;
        }
    }
    return 0;
