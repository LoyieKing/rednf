// ============================================================================
// DB_LoadInventory::_GetTagCharacInven @0x0840aba2（static bool）
// inventory 表 equipslot 列 → get_compressed_blob_data 解压进
// TAG_CHARAC_INVEN_DATA+0x262（cap 0x2dc，记录 0x3d）。错误行号 0x23e2/0x23e8/
// 0x23f1。
// ============================================================================
    // N(新候选)封顶：三个兄弟 if 块内 cMyTrace 槽深分配方向跨版本相反
    // （4.1 先声明者深：ORIG tt1 -0x44/tt2 -0x34/tt3 -0x24；4.4 反向：
    // 我方 tt1 -0x24/tt3 -0x44）。探针 /tmp/l22p/scope_probe.cpp 两形不可翻。
bool DB_LoadInventory::_GetTagCharacInven(unsigned int characNo,
                                          TAG_CHARAC_INVEN_DATA* out)
{
    // 声明序=槽深序（4.4 先声明→更深，实测）：db -0x14 / col -0x10 / ok -0xc
    // （ORIG 08409xxx 区实证 mov%eax,-0x14 / movl$0,-0x10 / mov%al,-0x9）。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3,
                                                  (ENUM_SERVER_GROUP)0);
    int col;
    bool ok;
    db->set_query("seLect equipslot from inventory where charac_no = %d",
                  characNo);
    // ORIG 08409xxx：exec/fetch 直 xor 判（无 -0x9 保存）
    if (!db->exec(true))
    {
        // P221：具名 cMyTrace 在兄弟 if 块的槽深方向跨版本相反；全表达式
        // 临时链复现 ORIG（tt1 -0x44 / tt2 -0x34 / tt3 -0x24）。
        cMyTrace(__PRETTY_FUNCTION__, 0x23e2, 5)
            ("DB_LoadInventory::_GetTagCharacInven, exec ERROR charac_no=%u",
             characNo);
        return 0;
    }
    if (!db->fetch())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x23e8, 5)
            ("DB_LoadInventory::_GetTagCharacInven, fetch ERROR charac_no=%u",
             characNo);
        return 0;
    }
    // TAG_CHARAC_INVEN_DATA equipslot 区：ORIG `add $0x262` = 第 10 槽起。
    // ORIG 8409：col 入 -0x10 槽后传值（非字面量、无自增）。
    col = 0;
    ok = get_compressed_blob_data(db, col,
                                  reinterpret_cast<char*>(&out->m_items[10]),
                                  0x2dc, 0x3d);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x23f1, 5)
            ("DB_LoadInventory::GetUserInven, get_compressed_blob_data ERROR"
             " charac_no=%u", characNo);
        return 0;
    }
    return 1;
}