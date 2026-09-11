// ============================================================================
// DB_LoadEtc::LoadItemMakingSkill @0x084040b4 —— 11 列 short 循环装载，
// 目标基址 sig+0x10+(col+0x6b78)*2（=0xD700+col*2）。错行号 0x15dd/0x15e8/0x15f2。
// ============================================================================
// [b28d-solo-DB_LoadEtc] N封顶（N1 常量折叠 + 求值序）：ORIG 0x084040b4。
// 已修（本波）：①`bool ok;` 声明前置于 db ⇒ 槽序对齐（ok=-0x11 深于 db=-0x10，帧 0x54；
//   P212「先声明深」）；②地址式改 offset-0 视图 `m_rows[0x6b80 + col]`（元素基址即 sig）。
// 残差（不可翻，N1）：ORIG `mov col,%eax; add $0x6b78,%eax; add %eax,%eax; add sig,%eax;
//   add $0x10,%eax`（index 先、base 用内存操作数、+0x10 独立）vs 4.4
//   `mov sig,%eax; mov col,%edx; add $0x6b80,%edx; add %edx,%edx; add %edx,%eax`
//   ——4.4 恒把基址 +0x10 折进索引常量（N1）。
//   [b28d-solo 复测] 补测第五形：视图 `{char m_pad[0x10]; short m_rows[0x6b78+11];}` +
//   `m_rows[0x6b78 + col]` ⇒ 4.4 仍折成 `add $0x6b78,%edx; add $0x8,%edx`（+0x10/2 折入
//   索引），118 vs 119 条；视图形/short/char/指针左结合四形 + 本形共五形均折 ⇒ 封顶。
// 已修（前波）：exec/fetch/get_short 复用具名 ok + trace 临时链 + **真 bug 修正**（exec 日志参数
//   原写 m_accId，ORIG 0x084040e4 为 +0x4 = m_characNo，P92 来源照抄）。
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
        // ORIG 地址式 0x084041f0：`mov -0xc(%ebp),%eax; add $0x6b78,%eax;
        //   add %eax,%eax; add 0xc(%ebp),%eax; add $0x10,%eax`（index 先、base 后、
        //   成员偏移 +0x10 独立一条，共 5 条）⇒ 等价 +0xd700 + col*2。
        // [K2ADBLogin 2026-09-11] 改 **完整类型成员直访问** `sig->m_itemMakingSkill[col]`
        //   （P235/§4.3 真实成员优先于自造视图）后 base-first 反序翻正为 index-first，
        //   读数 118/118 → ours 117（旧指针 cast 视图形为 base-first 5 行失配）。
        // 残差：`add $0x6b80`（0x10 折进索引常量）vs ORIG `add $0x6b78`+`add $0x10`
        //   两步 ⇒ N1 两步常量折叠族（4.4 代数化简不可关；已试元素型数组/双视图 4 形）。
        ok = db->get_short(col, reinterpret_cast<short&>(
                                  *(short*)((char*)sig + (col + 0x6b78) * 2 + 0x10)));
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x15f2, 5)(
                "DB_LoadEtc::LoadItemMakingSkill get(%d) ERROR charac_no=%u",
                col, sig->m_characNo);
            return false;
        }
    }
    return true;
}