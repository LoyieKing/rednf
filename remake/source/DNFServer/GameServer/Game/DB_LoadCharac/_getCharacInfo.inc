// ============================================================================
// DB_LoadCharac::_getCharacInfo @0x08408cf4
// charac_info 33 列装载：get_n_rows()==1 才继续；exec/fetch/任一 get 失败
// 返 0；col32(guild_secede) 后按 tag_charac_no(+0x686) 非零装载
// TAG_CHARAC_DATA(+0x68b)（_GetTagCharacInfo 成功才置 1）。
// ============================================================================
bool DB_LoadCharac::_getCharacInfo(SIG_LOAD_CHARAC* sig)
{
    MySQL* db = _getGameDB();
    db->set_query(
        "seLect charac_no,charac_name,job,lev,grow_type,maxHP,maxMP,phy_attack,"
        "phy_defense,mag_attack,mag_defense,inven_weight,hp_regen,mp_regen,"
        "move_speed, attack_speed, cast_speed, hit_recovery, jump, charac_weight,"
        " max_fatigue, create_time,guild_id,guild_right,max_premium_fatigue,"
        "member_flag,element_resist,spec_property,m_id,expert_job, "
        "skill_tree_index, event_charac_level, guild_secede from charac_info "
        "where charac_no=%d and delete_flag!=1",
        sig->m_characNo);
    // [b28d-solo] 负极性短路平铺形（/tmp/p_if.cpp f1 探针实证）：4.4 复现
    // ORIG 0x08408d38 的 `jne mov$1 / je mov$0 / test;je` 汇聚块；
    // 正极性包裹 `if (exec && fetch)` 产 f2 形（je/je 同落点 + 成功标志）⇒ DIFF。
    if (!db->exec(true) || !db->fetch())
        return false;
    if (db->get_n_rows() != 1)
        return false;

    // 逐列 get_* 引用绑定具名字段（SigTypes_C.h union 别名类型 = 下列
    // ORIG callee 重载形参类型，_getCharacInfo.md 汇编逐条实证）。
    if (!db->get_int(0, sig->m_characData.m_characNo))       // charac_no（0811692c int&）
        return false;
    if (!db->get_str(1, sig->m_characData.m_name, 0x1d))     // charac_name
        return false;
    if (!db->get_byte(2, sig->m_characData.m_job))           // job（080fde54 char&）
        return false;
    if (!db->get_short(3, sig->m_characData.m_lev))          // lev（080fddec short&）
        return false;
    if (!db->get_byte(4, sig->m_characData.m_growType))      // grow_type（char&）
        return false;
    if (!db->get_uint(5, sig->m_characData.m_add.m_maxHP))   // maxHP（080e22f2 uint&）
        return false;
    if (!db->get_uint(6, sig->m_characData.m_add.m_maxMP))   // maxMP
        return false;
    if (!db->get_ushort(7, sig->m_characData.m_add.m_field08))   // phy_attack（ushort&）
        return false;
    if (!db->get_ushort(8, sig->m_characData.m_add.m_field0a))   // phy_defense
        return false;
    if (!db->get_ushort(9, sig->m_characData.m_add.m_field0c))   // mag_attack
        return false;
    if (!db->get_ushort(10, sig->m_characData.m_add.m_field0e))  // mag_defense
        return false;
    if (!db->get_int(11, sig->m_characData.m_add.m_field3a))     // inven_weight（int&）
        return false;
    if (!db->get_short(12, sig->m_characData.m_add.m_hpRegen))   // hp_regen（short&）
        return false;
    if (!db->get_short(13, sig->m_characData.m_add.m_mpRegen))   // mp_regen（short&）
        return false;
    if (!db->get_uint(14, sig->m_characData.m_add.m_moveSpeed))  // move_speed（uint&）
        return false;
    if (!db->get_ushort(15, sig->m_characData.m_add.m_field46))  // attack_speed
        return false;
    if (!db->get_ushort(16, sig->m_characData.m_add.m_field48))  // cast_speed
        return false;
    if (!db->get_short(17, sig->m_characData.m_add.m_hitRecovery)) // hit_recovery（short&）
        return false;
    if (!db->get_short(18, sig->m_characData.m_add.m_jump))  // jump（short&）
        return false;
    if (!db->get_int(19, sig->m_characData.m_add.m_field4e)) // charac_weight（int&）
        return false;
    if (!db->get_ushort(20, sig->m_characData.m_field2e))    // max_fatigue（ushort&）
        return false;
    if (!db->get_str(21, sig->m_characData.m_field34, 0x18)) // create_time
        return false;
    if (!db->get_uint(22, sig->m_characData.m_guildIdUint))  // guild_id（uint&）
        return false;
    if (!db->get_ubyte(23, sig->m_characData.m_field5a))     // guild_right（uchar&）
        return false;
    if (!db->get_ushort(24, sig->m_characData.m_field5d))    // max_premium_fatigue
        return false;
    if (!db->get_ubyte(25, sig->m_characData.m_field5f))     // member_flag
        return false;
    if (!db->get_binary(26, sig->m_characData.m_add.m_elementResist, 8)) // element_resist
        return false;
    if (!db->get_binary(27, sig->m_characData.m_add.m_specProperty, 0x22)) // spec_property
        return false;
    if (!db->get_uint(28, sig->m_characData.m_unkB2.m_mId))  // m_id（uint&）
        return false;
    // expert_job：ORIG @0x0844d73a = MySQL::get_byte(int, int&)（非 char& 重载）
    // ——宽度修正：绑定 int m_field106（原 *(char*) 绑到 0x080fde54，callee 不符）。
    if (!db->get_byte(29, sig->m_characData.m_unkB2.m_field106)) // expert_job（int&）
        return false;
    if (!db->get_byte(30, sig->m_characData.m_unkB2.m_skillTreeIndex)) // skill_tree_index（char&）
        return false;
    if (!db->get_byte(31, sig->m_characData.m_unkB2.m_eventCharacLevel)) // event_charac_level（char&）
        return false;
    if (!db->get_byte(32, sig->m_characData.m_unkB2.m_b124)) // guild_secede（char&）
        return false;

    if (sig->m_tagCharacNo != 0)
    {
        // [b28d-solo] ORIG 0x084093db `mov %al,-0x9(%ebp)` 具名 bool 局部，
        // 失败路径 `movzbl -0x9,%eax; jmp`（返回该局部值，非 mov$0）。
        bool ok = _GetTagCharacInfo(sig->m_tagCharacNo,
                                    &sig->m_tagCharacData);
        if (!ok)
            return ok;
    }
    return true;
}