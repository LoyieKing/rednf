// ============================================================================
// DB_LoadCharac::_GetTagCharacInfo @0x08409fa6（static，无 this）
// tag_charac_info 23 列装载：exec/fetch 失败或 getAffectedRowCount()==0 返 0；
// 任一 get 失败返 0；全部成功返 1。
// [b28d-solo] 373/373 AE：ORIG 每列 `mov -0xc,%eax; addl $1,-0xc`（自增列号
// 局部 col++）+ `lea 0x4(%eax),%edx`（m_name 显式 &out->m_name[0] 取址形）。
// ============================================================================
bool DB_LoadCharac::_GetTagCharacInfo(unsigned int characNo, TAG_CHARAC_DATA* out)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "seLect charac_no, charac_name, job, lev, grow_type, skill_tree_index, "
        "maxHP, maxMP, phy_attack, phy_defense, mag_attack, mag_defense, "
        "element_resist, spec_property, inven_weight, hp_regen, mp_regen, "
        "move_speed, attack_speed, cast_speed, hit_recovery, jump, "
        "charac_weight from charac_info where charac_no = %d and delete_flag != 1",
        characNo);
    if (!db->exec(true) || !db->fetch())
        return false;
    if (db->getAffectedRowCount() == 0)
        return false;

    // 绑定字段类型 = _GetTagCharacInfo.md 汇编各 callee 重载形参（同
    // _getCharacInfo 口径；m_add 区共用 CCharacter.h _Additioal_info 别名）。
    // [b28d-solo] ORIG 0x0840a04c `movl $0x0,-0xc(%ebp)` + 每列
    // `mov -0xc,%eax; addl $1,-0xc; …mov %eax,0x4(%esp)` ⇒ 源码用自增列号
    // 局部 `col++`（字面量列号每列少 2 条，gap 47 即此）。
    int col = 0;
    if (!db->get_int(col++, out->m_characNo))     // charac_no（0811692c int&）
        return false;
    // [b28d-solo] ORIG 0x0840a081 `mov 0xc(%ebp),%eax; lea 0x4(%eax),%edx`
    // （单段 lea）⇒ 显式取址形（数组名退化恒 add+mov 两段）。
    if (!db->get_str(col++, &out->m_name[0], 0x1d)) // charac_name
        return false;
    if (!db->get_byte(col++, out->m_job))         // job（080fde54 char&）
        return false;
    if (!db->get_short(col++, out->m_lev))        // lev（080fddec short&）
        return false;
    if (!db->get_byte(col++, out->m_growType))    // grow_type（char&）
        return false;
    if (!db->get_byte(col++, out->m_skillTreeIndex)) // skill_tree_index（char&）
        return false;
    if (!db->get_uint(col++, out->m_add.m_maxHP)) // maxHP（080e22f2 uint&）
        return false;
    if (!db->get_uint(col++, out->m_add.m_maxMP)) // maxMP
        return false;
    if (!db->get_ushort(col++, out->m_add.m_field08)) // phy_attack（ushort&）
        return false;
    if (!db->get_ushort(col++, out->m_add.m_field0a)) // phy_defense
        return false;
    if (!db->get_ushort(col++, out->m_add.m_field0c)) // mag_attack
        return false;
    if (!db->get_ushort(col++, out->m_add.m_field0e)) // mag_defense
        return false;
    if (!db->get_binary(col++, out->m_add.m_elementResist, 8)) // element_resist
        return false;
    if (!db->get_binary(col++, out->m_add.m_specProperty, 0x22)) // spec_property
        return false;
    if (!db->get_int(col++, out->m_add.m_field3a)) // inven_weight（int&）
        return false;
    if (!db->get_short(col++, out->m_add.m_hpRegen)) // hp_regen（short&）
        return false;
    if (!db->get_short(col++, out->m_add.m_mpRegen)) // mp_regen（short&）
        return false;
    if (!db->get_uint(col++, out->m_add.m_moveSpeed)) // move_speed（uint&）
        return false;
    if (!db->get_ushort(col++, out->m_add.m_field46)) // attack_speed
        return false;
    if (!db->get_ushort(col++, out->m_add.m_field48)) // cast_speed
        return false;
    if (!db->get_short(col++, out->m_add.m_hitRecovery)) // hit_recovery（short&）
        return false;
    if (!db->get_short(col++, out->m_add.m_jump)) // jump（short&）
        return false;
    if (!db->get_int(col++, out->m_add.m_field4e)) // charac_weight（int&）
        return false;
    return true;
}