void CMissionList_Charac::GM_SetMissionValue(int kind, int value)
{
    // b28c-L5：同 Remove_Mission —— ORIG 0x085e56ee 三岛形（+0x2f nop;jmp /
    // +0x32 nop fall），探针 l5_probe_a fD 复现（P223）。
    if (kind <= 0)
        goto L_nonrange;
    if (kind >= 0x24)
        goto L_overrange;
    m_missionList[kind].m_count = (short)value;
    alter();
L_overrange:;
L_nonrange:;
}