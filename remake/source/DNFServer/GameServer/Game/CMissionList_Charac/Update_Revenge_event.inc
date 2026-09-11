void CMissionList_Charac::Update_Revenge_event(CUser& user,
                                               PVP_BATTLE_MODE mode,
                                               bool win)
{
    // [b28d-64] P133 补/P223 BM8：ORIG 0x085e6229/0x085e623d 两守卫各自
    // `setne %al; test %al,%al; jne` 物化 + 独立 nop 岛（+0x143/+0x146），
    // 合并 && 被 4.4 折成直跳（少 setne 与两岛）。拆两条否定 goto + 尾标签串。
    // win 分支 ORIG 0x085e6244 `cmpb $0,-0x2c; je <清零块>` ⇒ 正向 if(win){alter}。
    // [b28d-solo-CMissionList_Charac 定稿] 原「issueMode 死局部」判词系**误读**：
    // ORIG 0x085e61c3 `mov %ax,-0x22(%ebp)` 的槽 = param 基址 -0x23 + 1 =
    // pack(1) 成员 m_victory@+0x01（MissionTypes.h 权威偏移），即原码是
    // `param.m_victory = getIssue_mode(mode == PVP_BATTLE_MODE_2);`（结果供
    // check_mission_kind 消费）。改成员赋值后帧/槽 out(-0x14)/param(-0x23) 与
    // ORIG 全等（同 Update_EnterWorld_event 的 m_channelTo/m_channelFrom 族）。
    if (G_GameWorld()->GetChannelType() != 0xf)
        goto L_notchannel;
    if (getIndex_byKind(0x1b) != 0)
        goto L_hasmission;
    {
        if (win)
        {
            alter();
            std::vector<int> out;
            MissionClearCondition_Parameter param(mode);
            param.m_victory = getIssue_mode(mode == PVP_BATTLE_MODE_2);
            check_mission_kind(0xc, user, param, out);
            check_mission_kind(0x16, user, param, out);
            Send_Cleared_Mission_List(user, out);
        }
        else
        {
            m_missionList[0xc].m_streak = 0;
            m_missionList[0x16].m_streak = 0;
        }
    }
L_hasmission:;
L_notchannel:;
}