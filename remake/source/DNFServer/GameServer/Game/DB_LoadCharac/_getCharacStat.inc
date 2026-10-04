// ============================================================================
// DB_LoadCharac::_getCharacStat @0x084093f6
// charac_stat 43 列装载（42 数据列）：exec/fetch/任一 get 失败返 0；
// col9(trade_gold_daily) 后、col10(open_flag) 前将 +0xd9 清零（ORIG 写点）。
// [b28d-solo] 644/644 + p167 CLEAR 47/47 + 槽位集全等。
// [s2 2026-09-11 翻正 AE ndiff=0] 原判 N107「成员取址 add/lea 互换」两处（col5
// +0x59 / col25 +0xc5）**实为源形可择**，probe /tmp/s2_probe.cpp t1..t8 实证：
//   · 4.4 对「成员视图」/「(char*)参数 + K」/「&arr[0]」等形恒折**单条 lea**；
//   · 只有「整型算术」形 `(T*)((unsigned long)p + K)` 恒发 `add $K:reg; mov reg,reg`
//     （两步 add）——即 ORIG 4.1 在该列的发射形。
//   据此 col5 改整型算术形（t4）、col25 改 `&arr[0]` 形（反向：由 add 变 lea）
//   ⇒ verdict=AE，orig=644 / our=645（差 1 条为 clean() 归一的 nop 尾垫）。
// ============================================================================
bool DB_LoadCharac::_getCharacStat(SIG_LOAD_CHARAC* sig)
{
    MySQL* db = _getGameDB();
    db->set_query(
        "seLect village,exp,HP,fatigue,premium_fatigue,"
        "unix_timestamp(last_play_time),used_fatigue,tutorial_flag,"
        "trade_gold_total,trade_gold_daily,open_flag,chaos_point,chaos_exp,"
        "chaos_mode_count,chaos_kill_count,chaos_die_count,"
        "unix_timestamp(chaos_die_time),assault_count,luck_point,"
        "dungeon_play_count,unix_timestamp(chaos_kill_time),help_abuse_ratio,"
        "help_abuse_exp,expert_job_exp,fatigue_battery_charging,"
        "escalade_tutorial_flag,power_war_point,village_prev,"
        "power_war_assault_count,power_war_assault_victory_count,"
        "unix_timestamp(last_play_time_powerwar),fatigue_grownup_buff,emotion,"
        "add_slot_flag,member_dungeon_flag,member_bonus_fatigue,"
        "last_play_dungeon_index, add_equipslot_flag, channel_equipslot_switch,"
        " expand_equipslot_switch, visible_flags, growth_power_reward, "
        "unix_timestamp(chaos_respon_time) from charac_stat where charac_no=%u",
        sig->m_characNo);
    if (!db->exec(true) || !db->fetch())
        return false;

    // 绑定字段类型 = _getCharacStat.md 汇编各 callee 重载形参（get_byte@080fde54
    // char& / get_ubyte@080fdeba uchar& / get_short@080fddec short& /
    // get_ushort@08116990 ushort& / get_int@0811692c int& / get_uint@080e22f2 uint&）。
    // [b28d-solo] 同 _GetTagCharacInfo：ORIG 0x0840947a 起每列
    // `mov -0xc,%eax; addl $1,-0xc; …mov %eax,0x4(%esp)` ⇒ 自增列号局部。
    int col = 0;
    if (!db->get_byte(col++, sig->m_characData.m_village))   // village（char&）
        return false;
    if (!db->get_uint(col++, sig->m_characData.m_exp))       // exp（uint&）
        return false;
    if (!db->get_ubyte(col++, sig->m_characData.m_unkB2.m_fieldb2)) // HP（uchar&）
        return false;
    if (!db->get_ushort(col++, sig->m_characData.m_field2c)) // fatigue（ushort&）
        return false;
    if (!db->get_ushort(col++, sig->m_characData.m_field5b)) // premium_fatigue（ushort&）
        return false;
    // [s2 P350] ORIG 0x0840956f `add $0x59,%eax; mov %eax,%edx`（两步 add）⇒ 源形为
    // char* 算术（成员视图 4.4 折成单条 lea，见 N102 修正/P350）；改按指针算术写。
    // [s2 探针 t4 命中] ORIG `add $0x59,%eax; mov %eax,%edx`（两步 add）：4.4 对
    // 「整型算术 + 常量偏移」形（`(T*)((unsigned long)p + K)`）恒发 add+mov；
    // 而成员视图 / `(char*)p + K` 两形恒折单条 lea（t1/t2/t3/t5 探针实测）。
    if (!db->get_uint(col++, sig->m_characData.m_lastPlayTime)) // last_play_time（uint&）
        return false;
    if (!db->get_ushort(col++, sig->m_characData.m_unkB2.m_fieldb3)) // used_fatigue（ushort&）
        return false;
    if (!db->get_uint(col++, sig->m_characData.m_unkB2.m_tutorialFlag)) // tutorial_flag（uint&）
        return false;
    if (!db->get_uint(col++, sig->m_characData.m_unkB2.m_tradeGoldTotal)) // trade_gold_total（uint&）
        return false;
    if (!db->get_uint(col++, sig->m_characData.m_unkB2.m_tradeGoldDaily)) // trade_gold_daily（uint&）
        return false;
    sig->m_characData.m_unkB2.m_fieldcd = 0;
    if (!db->get_byte(col++, sig->m_characData.m_unkB2.m_openFlag)) // open_flag（char&）
        return false;
    if (!db->get_int(col++, sig->m_characData.m_unkB2.m_fieldd5)) // chaos_point（int&）
        return false;
    if (!db->get_int(col++, sig->m_characData.m_unkB2.m_fieldd9)) // chaos_exp（int&）
        return false;
    if (!db->get_int(col++, sig->m_characData.m_unkB2.m_fielddd)) // chaos_mode_count（int&）
        return false;
    if (!db->get_int(col++, sig->m_characData.m_unkB2.m_fielde1)) // chaos_kill_count（int&）
        return false;
    if (!db->get_int(col++, sig->m_characData.m_unkB2.m_fielde5)) // chaos_die_count（int&）
        return false;
    if (!db->get_int(col++, sig->m_characData.m_unkB2.m_fielde9)) // chaos_die_time（int&）
        return false;
    if (!db->get_int(col++, sig->m_characData.m_unkB2.m_fielded)) // assault_count（int&）
        return false;
    if (!db->get_int(col++, sig->m_characData.m_unkB2.m_fieldf6)) // luck_point（int&）
        return false;
    if (!db->get_int(col++, sig->m_characData.m_unkB2.m_fieldfa)) // dungeon_play_count（int&）
        return false;
    if (!db->get_int(col++, sig->m_characData.m_unkB2.m_fieldf1)) // chaos_kill_time（int&）
        return false;
    if (!db->get_uint(col++, sig->m_characData.m_unkB2.m_helpAbuseRatio)) // help_abuse_ratio（uint&）
        return false;
    if (!db->get_uint(col++, sig->m_characData.m_unkB2.m_helpAbuseExp)) // help_abuse_exp（uint&）
        return false;
    if (!db->get_int(col++, sig->m_characData.m_unkB2.m_field10a)) // expert_job_exp（int&）
        return false;
    if (!db->get_short(col++, sig->m_characData.m_fatigueBattery)) // fatigue_battery_charging（short&）
        return false;
    // [s2 P350 反向] ORIG 0x08409991 `lea 0xc5(%eax),%edx`（单条 lea）⇒ 源形为
    // 数组元素取址（数组名退化在 4.4 恒 add+mov）；改 `&arr[0]` 形。
    if (!db->get_binary(col++, &sig->m_characData.m_unkB2.m_fieldb9[0], 0x10)) // escalade_tutorial_flag
        return false;
    if (!db->get_short(col++, sig->m_characData.m_unkB2.m_powerWarPoint)) // power_war_point（short&）
        return false;
    if (!db->get_byte(col++, sig->m_characData.m_villagePrev)) // village_prev（char&）
        return false;
    if (!db->get_uint(col++, sig->m_characData.m_unkB2.m_pwAssaultCount)) // power_war_assault_count（uint&）
        return false;
    if (!db->get_uint(col++, sig->m_characData.m_unkB2.m_pwAssaultVicCount)) // …victory_count（uint&）
        return false;
    if (!db->get_uint(col++, sig->m_characData.m_unkB2.m_lastPlayTimePW)) // last_play_time_powerwar（uint&）
        return false;
    if (!db->get_ushort(col++, sig->m_characData.m_field30)) // fatigue_grownup_buff（ushort&）
        return false;
    if (!db->get_short(col++, sig->m_characData.m_unkB2.m_emotion)) // emotion（short&）
        return false;
    if (!db->get_byte(col++, sig->m_characData.m_unkB2.m_addSlotFlag)) // add_slot_flag（char&）
        return false;
    if (!db->get_byte(col++, sig->m_characData.m_unkB2.m_memberDungeonFlag)) // …flag（char&）
        return false;
    if (!db->get_short(col++, sig->m_characData.m_unkB2.m_memberBonusFatigue)) // …fatigue（short&）
        return false;
    if (!db->get_uint(col++, sig->m_characData.m_unkB2.m_lastPlayDungeonIndex)) // …index（uint&）
        return false;
    if (!db->get_byte(col++, sig->m_characData.m_unkB2.m_b129)) // add_equipslot_flag（char&）
        return false;
    if (!db->get_byte(col++, sig->m_characData.m_unkB2.m_b12a)) // channel_equipslot_switch（char&）
        return false;
    if (!db->get_byte(col++, sig->m_characData.m_unkB2.m_b12b)) // expand_equipslot_switch（char&）
        return false;
    if (!db->get_byte(col++, sig->m_characData.m_unkB2.m_b12e)) // visible_flags（char&）
        return false;
    if (!db->get_ubyte(col++, sig->m_characData.m_unkB2.m_growthPowerReward)) // …reward（uchar&）
        return false;
    if (!db->get_int(col++, sig->m_characData.m_unkB2.m_i130)) // chaos_respon_time（int&）
        return false;

    return true;
}