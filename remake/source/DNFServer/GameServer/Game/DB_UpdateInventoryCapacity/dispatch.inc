// ============================================================================
// DB_UpdateInventoryCapacity::dispatch @ 0x08444bea
// mangled _ZN26DB_UpdateInventoryCapacity8dispatchEiiP6Stream
// ============================================================================
// 报告 docs/class_func_reports/DB_UpdateInventoryCapacity/dispatch.md 全文：
// 解包 serverGroup(int) → GetDBHandle(DB_HANDLE_3, serverGroup) → 解包
// characNo(uint)/capacity(int) → set_query("upDate inventory set
// inventory_capacity=%d where charac_no=%u", capacity, characNo) → exec(true)；
// 失败或 affectedRowCount==0 → cMyTrace(__PRETTY_FUNCTION__, 0xabc8, 5) 记录同
// 款 SQL 后返 0；成功返 1。
// [b28d-lane6] AE：三处源形修复 —— ①失败条件写负极性短路
// `exec(true)!=1 || getAffectedRowCount()==0`（复现 ORIG 0x08444c92 xor/or +
// mov $1/$0 物化，P214①）；②trace 改 P227 全表达式临时链（复现 ORIG 0x08444cba
// esi/ebx 跨 ctor 保活 + trace 槽 -0x1c）；③槽号/帧随二者自动对齐（0x30）。
bool DB_UpdateInventoryCapacity::dispatch(int /*userIdx*/, int /*slot*/,
                                          Stream* stream)
{
    int serverGroup = 0;      // -0x20（先于 GetDBHandle 解包）
    *stream >> serverGroup;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_3, static_cast<ENUM_SERVER_GROUP>(serverGroup));
    unsigned int characNo = 0;  // -0x24
    int capacity = 0;           // -0x28
    *stream >> characNo;
    *stream >> capacity;
    db->set_query("upDate inventory set inventory_capacity=%d where charac_no=%u",
                  capacity, characNo);
    // b28d-lane6：ORIG 0x08444c92 负极性短路 `exec!=1 || affected==0` → mov $1/$0
    // 物化后 test/je 返回 1（P214① 正极性反演的另一向）。
    if (db->exec(true) != 1 || db->getAffectedRowCount() == 0)
    {
        // P227 全表达式临时链（复现 ORIG 0x08444cba esi/ebx 跨 ctor 保活 + trace 槽 -0x1c）。
        cMyTrace(__PRETTY_FUNCTION__, 0xabc8, 5)(
            "upDate inventory set inventory_capacity=%d where charac_no=%u",
            capacity, characNo);
        return 0;
    }
    return 1;
}