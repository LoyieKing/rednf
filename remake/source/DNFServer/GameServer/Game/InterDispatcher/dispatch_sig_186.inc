// [b28d-lane2] verdict=DIFF ndiff=81（ORIG 0x084db294）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+7。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=111（ORIG 0x84db294）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
// [lane2 2026-09-10] verdict=DIFF→AE：① m_accIds[1] 视图数组（ORIG 084db31c `mov 0xd(%eax,%edx,4)` scaled）；
//   ② P227 两处 cMyTrace 全表达式临时链；③ P191 双初始化（ORIG 084db303/084db30a 两条 count 清零）；
//   ④ P223 空 then 占位岛（ORIG 084db323 `jbe <nop>`）用负条件 goto 复现。
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
}