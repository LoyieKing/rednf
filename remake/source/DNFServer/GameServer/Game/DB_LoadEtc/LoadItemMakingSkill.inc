bool DB_LoadEtc::LoadItemMakingSkill(SIG_LOAD_ETC* sig)
{
    // ORIG 槽序：ok(-0x11) 深于 db(-0x10) ⇒ 原码 ok 先声明（P212「先声明深」）。
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect weapon, cloth, leather, light_armor, heavy_armor, plate,"
                  " amulet, wrist, ring, support, magic_stone from"
                  " item_making_skill_info where charac_no=%u", sig->m_characNo);
    ok = db->exec(true);                                     // ORIG -0x11 具名槽（exec/fetch 复用）
    if (!ok)
    {
        // ORIG 0x084040e4：ctor 前把 m_accId 装 %ebx 跨 ctor 保活 ⇒ P227 临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x15dd, 5)(
            "DB_LoadEtc::LoadItemMakingSkill, exec() ERROR charac_no=%u",
            sig->m_characNo);            // ORIG +0x4 = characNo（P92 日志参数来源照抄）
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        return true;
    }
    ok = db->fetch();
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x15e8, 5)(
            "DB_LoadEtc::LoadItemMakingSkill fetch, exec() ERROR charac_no=%u",
            sig->m_characNo);
        return false;
    }

    for (int col = 0; col <= 0xa; ++col)                     // cmpl $0xa setle → ≤10
    {
        // ORIG 地址式：sig + 0x10 + (col + 0x6b78)*2 = +0xd700 + col×2。
        // [b28d-solo-DB_LoadEtc] 成员数组**基址在 +0x10**、索引含常量 0x6b78
        // （而非基址 0 + 索引 0x6b80）——4.4 保留独立的 `add $0x10,%eax`
        // （P30/P311 两步寻址；基址 0 形恒被折进索引常量 0x6b80）。
        // 字节视图 + 显式 ×2 索引，`+0x10` 后置：ORIG 形 `add $0x6b78; add %eax,%eax;
        // add sig; add $0x10`（5 条）。4.4 恒把 0x10 折进索引常量（`add $0x6b80`）
        // 并令 base 用 eax、索引用 edx ⇒ 残余 = N1 两步常量折叠族（已试 4 形，
        // 旧式元素型数组会额外多 1 条 `add $0x8`）。
        struct ItemMakingSkillView
        {
            char m_bytes[(0x6b78 + 11) * 2 + 0x10];
        };
        ok = db->get_short(col, *reinterpret_cast<short*>(
                &((ItemMakingSkillView*)sig)->m_bytes[(0x6b78 + col) * 2] + 0x10));
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x15f2, 5)(
                "DB_LoadEtc::LoadItemMakingSkill get(%d) ERROR charac_no=%u",
                col, sig->m_characNo);
            return false;
        }
    }
    return true;
