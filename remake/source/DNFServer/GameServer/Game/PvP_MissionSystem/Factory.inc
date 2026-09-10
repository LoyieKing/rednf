CMission* PvP_MissionSystem::Factory(const MissionScript& script)
{
    CMission* result = 0;
    switch (script.m_field34)
    {
    case 0:   result = new CMission_winning_count(script, script.m_field7c); break;
    case 1:   result = new CMission_winning_streak(script, script.m_field7c, script.m_field80); break;
    case 2:   result = new CMission_join(script, script.m_field7c); break;
    case 3:   result = new CMission_kill_count(script, script.m_field7c); break;
    case 4:   result = new CMission_hereafter_join(script, script.m_field7c); break;
    case 5:   result = new CMission_hereafter_winning_count(script, script.m_field7c); break;
    case 6:   result = new CMission_remain_hp(script, script.m_field7c, script.m_field80); break;
    case 7:   result = new CMission_revenge(script, script.m_field7c); break;
    case 8:   result = new CMission_hereafter_kill_count(script, script.m_field7c); break;
    case 9:   result = new CMission_kill(script, script.m_field7c, script.m_field80); break;
    case 0xb: result = new CMission_within(script, script.m_field7c, script.m_field80, script.m_field84); break;
    case 0xc: result = new CMission_remains_hp(script, script.m_field7c, script.m_field80); break;
    case 0xd: result = new CMission_move_channel(script, script.m_field7c); break;
    case 0xe: result = new CMission_move_channel_total(script, script.m_field7c); break;
    case 0xf: result = new CMission_combo_clear(script, script.m_comboMap); break;
    // b28d-61 [P171 照抄不写]：ORIG 0x85e213b 入口已 `movl $0x0,-0x1c`，default
    // 无赋值直落 return；旧 `default: result = 0;` 多出 `movl $0x0,-0x1c`+jmp
    // （384 vs 382）。
    default:  break;
    }
    return result;
