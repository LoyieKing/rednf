void CMissionList_Charac::ClearMissionKind(CUser& user, short kind)
{
    // b28c-L5：同 Remove_Mission —— ORIG 0x085e63d6 两失败出口各落独立 nop 中继岛
    // （+0x40 nop;jmp / +0x43 nop fall），探针 l5_probe_a fD 形复现（P223）。
    if (kind <= 0)
        goto L_nonrange;
    if (kind >= 0x24)
        goto L_overrange;
    ClearMission(user, m_missionList[kind].m_index);
L_overrange:;
L_nonrange:;
}