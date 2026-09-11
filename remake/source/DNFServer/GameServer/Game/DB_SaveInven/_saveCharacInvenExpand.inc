// ===========================================================================
// _saveCharacInvenExpand — 报告 @0x08416b38（const）
// cargo(+0x895d,0x2438)/jewel(+0x40ac,0xf03) 压缩失败返 0；
// upDate charac_inven_expand 后直接返回 exec 结果。
// ===========================================================================
bool DB_SaveInven::_saveCharacInvenExpand(SIG_SAVE_INVENTORY* sig) const
{
    static const int handleIdx = 3;
    // §4.3 裸偏移清零：五段压缩快照的起点全部落在 SIG_SAVE_INVENTORY 的具名
    // 成员上（SigTypes_E.h:106-112，sizeof(Inven_Item)==0x3d 由
    // SIG_SAVE_EXPAND_EQUIPSLOT 0x24×0x3d==0x894 实证）：
    //   cargo  +0x895d = m_cargo（长度 sourceCargoSize 0x2438 → +0xad95 =
    //          m_avatarCoin，首尾皆具名闭合）
    //   jewel  +0x40ac = m_invenType1 + sourceInvenSize(0x3b55)/0x3d = 下标 249
    //          （珠宝区紧接背包快照之后；ORIG T 0x08416b38 @报告 const 版）
    char cargo[0x2b82] = { 0 };
    unsigned int cargoLen = 0x2b82;
    if (compress_zip(cargo, cargoLen, (char*)sig->m_cargo,
                     ARAD::DATABASE::sourceCargoSize) != 1)
        return false;
    char jewel[0x120f] = { 0 };
    unsigned int jewelLen = 0x120f;
    if (compress_zip(jewel, jewelLen,
                     (char*)&sig->m_invenType1[
                         ARAD::DATABASE::sourceInvenSize / sizeof(Inven_Item)],
                     ARAD::DATABASE::sourceJewelSize) != 1)
        return false;

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, E_SERVER_GROUP_NONE);
    db->set_query("upDate charac_inven_expand set cargo_capacity=%d,cargo='%s',jewel='%s' where charac_no=%u",
                  sig->m_cargoCapacity,
                  db->blob_to_str(0, cargo, cargoLen),
                  db->blob_to_str(1, jewel, jewelLen),
                  sig->m_characNo);
    return db->exec(true);
}