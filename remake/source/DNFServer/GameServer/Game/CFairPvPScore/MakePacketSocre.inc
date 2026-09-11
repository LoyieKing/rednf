// ---- ORIG 0x84a5cc2 MakePacketSocre(PacketGuard&, int) const ---------------
// b28d-solo-CFairPvPScore 判词：AE/IDENTICAL（已转出 TU DIFF 列表；233=233，
// 归一化 NDIFF=0）。
// 修法：①P257 正向包裹 `if (mode == 1) { … }`（旧 `if (mode != 1) return;` 在
// 4.4 多出 `jmp <epilogue>; nop` 尾岛，ORIG 084a5e5b 为 `cmpl $1; jne <尾>`）；
// ②字段 unsigned 化（m_modeStats/m_field40/m_field44/m_winStreak/m_bestWinStreak
// ⇒ put_int 实参复现 ORIG 两段 `mov field,%eax; mov %eax,%edx`）。
// p167 CLEAR（37=37）。
void CFairPvPScore::MakePacketSocre(PacketGuard& pkt, int mode) const
{
    pkt.put_byte(mode);
    pkt.put_int(m_modeStats[0][0]);
    pkt.put_int(m_modeStats[1][0]);

    int winCnt = 0;
    int loseCnt = 0;
    int drawCnt = 0;
    for (std::list<unsigned char>::const_iterator it = m_recentResults_.begin();
         it != m_recentResults_.end(); ++it)
    {
        if (*it == 1)
            ++winCnt;
        else if (*it == 2)
            ++loseCnt;
        else if (*it == 3)
            ++drawCnt;
    }
    pkt.put_byte(winCnt);
    pkt.put_byte(loseCnt);
    pkt.put_byte(drawCnt);
    pkt.put_int(m_winStreak);
    pkt.put_int(m_field40);
    pkt.put_int(m_field44);
    pkt.put_int(m_bestWinStreak);

    // ORIG 084a5e5b `cmpl $1; jne <尾 leave/ret>` ⇒ 正向包裹（早返形多 jmp+nop 岛，P257）。
    if (mode == 1)
    {
        pkt.put_int(m_modeStats[0][1]);
        pkt.put_int(m_modeStats[0][2]);
        pkt.put_int(m_modeStats[1][1]);
        pkt.put_int(m_modeStats[1][2]);
        pkt.put_byte((int)m_jobScore_.size());
        for (std::map<JobKey, FAIR_PVP_SCORE_MODE_COUNT>::const_iterator it =
                 m_jobScore_.begin();
             it != m_jobScore_.end(); ++it)
        {
            pkt.put_byte(it->first.m_job);                     // ORIG emit dword 低字节
            pkt.put_byte((int)(signed char)it->first.m_sub);   // ORIG movsbl
            pkt.put_int(it->second.m_cnt0);
            pkt.put_int(it->second.m_cnt1);
            pkt.put_int(it->second.m_cnt2);
        }
    }
}