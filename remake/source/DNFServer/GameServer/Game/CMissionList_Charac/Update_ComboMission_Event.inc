void CMissionList_Charac::Update_ComboMission_Event(CUser& user,
                                                    int comboIdx,
                                                    short count)
{
    // [b28d-solo-CMissionList_Charac] 本轮修形：①P223 三独立岛（ORIG
    // 0x085e682a/682d/6830 三个 nop;jmp 落点）⇒ 两条负向 goto + 尾标签串；
    // ②c 移入 if 块内（ORIG 0x085e6689 赋值在守卫之后）。
    if (count < 0)
        goto L_neg;
    if (count > 8)
        goto L_over;
    {
    if ((comboIdx == 0x1f) || (comboIdx == 0x20))
    {
        short c = count;
        short value = m_missionList[comboIdx].m_count;
        if (count > 3)
        {
            c = (short)(c - 4);
            value = m_missionList[comboIdx].m_streak;
        }
        // [b28d-solo-CMissionList_Charac] 本轮修复：c/value/nib 槽位与三守卫岛
        // 全部对齐（c -0xe / value -0xc / nib -0xa；js/jg/jne 三岛 0x1d4/0x1d7/0x1d8）；
        // 判词 N104（150/149）：唯一残差=EH 正常路径尾块与 unwind 块相对序
        //（ORIG 正常 ~vector 在 jmp 前、unwind 居中、三岛收尾；4.4 相反）并少 1 岛；
        // 槽位集/调用点差集全等（p167 9/9 CLEAR），语义等价。本轮复验：尾标签序
        // L_kind/L_over/L_neg 与 L_neg/L_kind/L_over 两序实测均不改块布局，
        // 4.4 恒 2 岛（jg 岛缺 jmp、jne 直落 epilogue）⇒ 与 ORIG 3 岛不可合。
        short nib = (short)(((int)value >> ((3 - (int)c) * 4)) & 0xf);
        value = (short)(value -
                        (short)((int)(short)nib << ((3 - (int)c) * 4)));
        nib = (short)(nib - 1);
        if (nib < 0)
        {
            nib = 0;
        }
        nib = (short)((int)nib << ((3 - (int)c) * 4));
        value = (short)(value + (short)nib);
        if (count > 3)
        {
            m_missionList[comboIdx].m_streak = value;
        }
        else
        {
            m_missionList[comboIdx].m_count = value;
        }

        MissionClearCondition_Parameter param(PVP_BATTLE_MODE_1);
        param.m_mode = 1;
        std::vector<int> out;
        check_mission_kind(comboIdx, user, param, out);
        if (!out.empty())
        {
            Send_Cleared_Mission_List(user, out);
        }
        alter();
    }
    }
L_kind:;
L_over:;
L_neg:;
}