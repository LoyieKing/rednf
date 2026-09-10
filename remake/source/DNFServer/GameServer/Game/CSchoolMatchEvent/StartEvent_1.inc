void CSchoolMatchEvent::StartEvent(Word_Param param)
{
    // ORIG 0x08273fec。Word_Param 4 字节按字节视图读 4 个配置值：
    //   byte0 channel_no / byte1 player_count / byte2 school_rating / byte3 player_rating。
    // [b28c-27] N82/N93：两处 cMyTrace 全表达式临时链槽 ORIG -0x54/-0x44，4.4 恒
    // -0x50/-0x40（差 4；类内对齐选择跨版本差）；findValue 经 P212 提至函数头后
    // 已落 ORIG -0x30 独立槽。其余为 .o 重定位伪差（AE 口径）。
    SetEventFlag(true);

    struct Bytes { unsigned char b0, b1, b2, b3; };   // 组件视图折叠 ORIG 单 disp（P1）
    Bytes* p = (Bytes*)&param;
    cMyTrace(__PRETTY_FUNCTION__, 0x1e, 0)
        ("CSchoolMatchEvent Start! channel_no(%d), player_count(%d), school_rating(%d), player_rating(%d)",
         (unsigned int)p->b0, (unsigned int)p->b1,
         (unsigned int)p->b2, (unsigned int)p->b3);

    // 仅学院频道（channel type 9/10）启动联赛匹配。
    if (G_GameWorld()->GetChannelType() == 9 ||
        G_GameWorld()->GetChannelType() == 10) {
        if (G_CEnvironment()->get_channel_no() == (int)p->b0) {
            CLeagueMatch::SetMatchAble(true);
            CLeagueMatch::SetPlayCountPerOneTeam((unsigned int)p->b1);
            PvPGlobalEnvironmentVariable::school_match_elo_k2_ = (int)p->b2;
            PvPGlobalEnvironmentVariable::school_match_elo_k_ = (int)p->b3;
            if (G_CEnvironment()->get_channel_no() == 0x47) {
                CLeagueMatch::SetBattleMode(2);
            }
            cMyTrace(__PRETTY_FUNCTION__, 0x30, 0)
                ("This is School Matching Server!!");
        }
    }

    // 当前频道号不在表内才登记。
    // b28d-35：find 的 key 用内联实参（临时槽），不设具名 findValue 局部——
    //   具名局部使 4.4 把 trace 对象整体浅移 4（-0x50/-0x40 vs ORIG -0x54/-0x44）。
    std::vector<int>::const_iterator it =
        std::find(m_channels.begin(), m_channels.end(), (int)p->b0);
    if (it == m_channels.end()) {
        m_channels.push_back((int)p->b0);
    }
