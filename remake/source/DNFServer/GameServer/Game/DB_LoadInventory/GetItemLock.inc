// ============================================================================
// DB_LoadInventory::GetItemLock @0x0840d766（static unsigned int）
// charac_item_lock_info.item_lock_info blob → get_binary(0, 0x3cc) →
// uncompress_zip 进 out（stItemLockInfo）；解压后尺寸 mod 8 != 0 且非 0
// → 0x2931。返回 0=OK，其余为 DBThread.cpp 行号（0x291a/0x2924/0x292b/
// 0x292f/0x2931）。
// b28d-solo-DB_LoadInventory：AE（137=137 仅重定位差）。修法 = 独立
// `char buf[0x3cc]={0}`（rep stos 0xf3 精确）+ 独立 outLen（取址折叠
// lea -0x48，消旧结构体形的 N78 计数差与 add 两步）+ P221 全表达式临时链
// （3 处 trace 槽 -0x44/-0x34/-0x24）+ get_binary 判空内联（ORIG 无 ok 槽）。
// ============================================================================
unsigned int DB_LoadInventory::GetItemLock(unsigned int characNo,
                                           stItemLockInfo* out)
{
    static const int handleIdx = 3;
    // ORIG 槽距：buf@-0x414（rep stos 0xf3）、outLen@-0x48（独立标量 ⇒ 取址
    // 单条 lea）、ok=-0x11（exec/fetch 保存读回位）。
    char buf[0x3cc] = {0};
    unsigned int outLen = 0x3cc;
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect item_lock_info  from charac_item_lock_info"
                  " where charac_no=%u", characNo);
    ok = db->exec(true);
    if (!ok)
    {
        // P221：互斥 if 块内具名 cMyTrace 的槽深方向跨版本相反（ORIG 先出现者
        // 最深 -0x44/-0x34/-0x24；4.4 具名恒反向）⇒ 全表达式临时链复现。
        cMyTrace(__PRETTY_FUNCTION__, 0x2919, 5)
            ("DB_LoadInventory::GetItemLock, exec() ERROR charac_no=%u", characNo);
        return 0x291a;
    }
    if (db->get_n_rows() == 0)
    {
        return 0;
    }
    ok = db->fetch();
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x2923, 5)
            ("DB_LoadInventory::GetItemLock, fetch() ERROR charac_no=%u", characNo);
        return 0x2924;
    }
    unsigned int binLen = db->get_binary_length(0);
    // ORIG 第三实参 `mov -0x48,%eax` = outLen 变量（非字面量）。
    // ORIG 此处直接 `xor $0x1,%eax`（无 ok 槽保存）⇒ 判空内联，勿经具名 ok。
    if (!db->get_binary(0, buf, outLen))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x292a, 5)
            ("DB_LoadInventory::GetItemLock, get_binary(2) ERROR charac_no=%u",
             characNo);
        return 0x292b;
    }
    if (!uncompress_zip(reinterpret_cast<char*>(out), outLen, buf,
                        binLen))
    {
        return 0x292f;
    }
    if ((outLen & 7) != 0 && outLen != 0)
    {
        return 0x2931;
    }
    return 0;
}