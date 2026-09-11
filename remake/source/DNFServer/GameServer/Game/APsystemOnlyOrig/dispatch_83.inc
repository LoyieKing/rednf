// ============================================================================
// DB_UpdateCreatureItemList::dispatch —— ORIG 0x0841fd12（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_3。GetOutBuffer<SIG_CREATURE_ITEM_LIST>（首 int=条目数；+0x21a4 起
// 行数组，步长 0x38，见 DB_CreatureItemListEntry）→ 逐行
// upDate creature_items set exp=%d,stomach=%d where ui_id=%d（实参序
// exp/stomach/ui_id）；任一 exec 失败返 0；全部成功返 1。
// [b28d-4] DIFF：P210/N82：帧继承差（ORIG push/pop callee-saved vs 4.4 leave/槽化）；ORIG 0841fd12
bool DB_UpdateCreatureItemList::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    SIG_CREATURE_ITEM_LIST* raw = stream->GetOutBuffer<SIG_CREATURE_ITEM_LIST>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    // [SweepIntervalSet] P0：ORIG 0x0841fdec 循环条件每次重读 `raw->m_count`
    // （`mov -0x14(%ebp),%eax; mov (%eax),%eax; cmp -0xc(%ebp),%eax; setg`），
    // 无 count 具名槽 ⇒ 删缓存局部。
    for (int i = 0; i < raw->m_count; ++i)
    {
        // [SweepIntervalSet 复测] P183 三次内联重算（实参位各写一遍完整
        // raw+i*0x38+0x21a4）实测 our 78 / ndiff 72，劣于具名 e 的 our 60 /
        // ndiff 60（4.4 对 i*0x38 恒发 `lea (,%eax,8)→mov→sub→mov` 四条，
        // 每处多 2 条 regmove，属 N18a 后端寄存器角色差，非源形可控）⇒ 保留 e。
        DB_CreatureItemListEntry* e = reinterpret_cast<DB_CreatureItemListEntry*>(
            reinterpret_cast<char*>(raw) + kCreatureItemRowsOffset + i * 0x38);
        db->set_query(
            "upDate creature_items set exp=%d,stomach=%d where ui_id=%d",
            e->m_exp, e->m_stomach, e->m_uiId);
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}