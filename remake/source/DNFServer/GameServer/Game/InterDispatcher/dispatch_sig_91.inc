// [b28d-lane2] verdict=DIFF ndiff=142（ORIG 0x084d0ba6）：寄存器中转(N18a/N7)+混合残差；gap+2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=56（ORIG 0x84d0ba6）：P30/N4 add-vs-lea。
int Inter_Monitor_RequestBlackListResult::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax; sete
        return 0;

    // [G2父·2026-09-11] 修正视图名拼写：权威定义（聚合器 2281 行）为
    //   Inter_Monitor_RequestBlackListResultPktView（Monitor_ 后带下划线）。
    Inter_Monitor_RequestBlackListResultPktView* pPkt =
        reinterpret_cast<Inter_Monitor_RequestBlackListResultPktView*>(data);
    // ORIG 084d1xxx：环体三处基址均从 **pPkt 槽**取（`add <S>,%eax`、
    //   `mov <S>,%ecx; mov 0xf(%eax,%ecx,1),%eax`），我方旧形重读 data 形参
    //   （`add 0x10(%ebp),%eax`）⇒ 原码用 pPkt 具名槽 + 索引形。
    PacketGuard packet;
    packet.put_header(1, 0x7b);
    packet.put_byte(1);
    if (pPkt->m_e <= 0xa)
    {
        packet.put_byte(
            (int)pPkt->m_e);
        for (int i = 0;
             i < (int)(unsigned int)pPkt->m_e;
             ++i)
        {
            const int nameLen =
                (int)strlen(pPkt->m_recs[i].m_name);
            if (nameLen == 0)
            {
                LogManager::logFormat(
                    1, "InterDispatcher.cpp",
                    __PRETTY_FUNCTION__,
                    0x2f6d, "BlackListNameLen Err charno(%d)",
                    user->getCurCharacNo());
            }
            packet.put_int((int)nameLen);
            packet.put_str(pPkt->m_recs[i].m_name,
                           (int)nameLen);
            struct tm tmVal;                               // P43：经返回值访问
            struct tm* ptm = localtime_r(
                &pPkt->m_recs[i].m_time, &tmVal);
            packet.put_byte(ptm->tm_year);
            packet.put_byte(ptm->tm_mon);
            packet.put_byte(ptm->tm_mday);
            user->registerToBlackList(
                pPkt->m_recs[i].m_id);
        }
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;

// [G2B-Monitor] 本件已做：环体三处基址改走 pPkt 槽（ORIG `add <S>,%eax`、
//   `mov <S>,%ecx; mov 0xf(%eax,%ecx,1),%eax`；我方旧形重读 `0x10(%ebp)` 形参）。
//   残余留 N：① `if (pPkt->m_e <= 0xa)` 的落点形（ORIG `jbe`+`mov $0x0,%ebx`+`jmp`
//   两跳 vs 我方单 `ja`）＝ return 值寄存（ebx）与分支布局差；② 尾部 call 与
//   `lea <S>,%eax; mov %eax,(%esp)` 的相对位移（ndiff 各 3~4）—— 均属语句块尾布局/
//   寄存器选择族 ⇒ 留 N。
}