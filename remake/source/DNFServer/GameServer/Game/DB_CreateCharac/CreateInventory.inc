// ===========================================================================
// CreateInventory — 报告 @0x08402230
// item==NULL 仅写 coin；否则将 item 起点 0x3b55 字节 zip 后入库 + 空背包扩容表。
// ===========================================================================
// b28d-solo-DB_CreateCharac verdict=AE ORIG 0x08402230 94=94。
// 修法：`char zipped[0x473e]={0}` 复现 4.1 memset call + size %edx 中转（破旧「待解」项）
// + P225 拆嵌套 if 消 && 物化 + P102 同一 bool ok + coin 内联实参（%ebx 跨 blob_to_str）。
bool DB_CreateCharac::CreateInventory(unsigned int characNo, Inven_Item* item)
{
    static const int handleIdx = 3;
    // N78/待解：ORIG 对 0x473e 大数组发 memset call 且 size 经 %edx 中转；
    // 试 `= {0}` 触发 4.1 同形（4.4 大数组可能同样退化为 call）。
    char zipped[0x473e] = {0};
    unsigned int zippedLen = 0x473e;
    // P225 推广：拆嵌套 if 消 4.4 的 && bool 物化（ORIG 两跳同一 L_skip）。
    if (item != NULL)
    {
        if (!compress_zip(zipped, zippedLen, reinterpret_cast<char*>(item), 0x3b55))
        {
            return false;
        }
    }

    // P0/P102：ORIG bool 槽 -0xd 两次复用（mov %al,-0xd; movzbl; xor; test; je）。
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    if (item == NULL)
    {
        // P0：ORIG call 结果直入实参槽（无具名 coin 局部）。
        db->set_query("inSert into inventory (charac_no, coin) values(%u, %d)",
                      characNo, WongWork::CEventMgr::GetCoinCharacCreate());
    }
    else
    {
        // P52/P227：最右实参先求值入 %ebx 跨 blob_to_str 保活（ORIG 084022eb）。
        db->set_query("inSert into inventory (charac_no, inventory,coin) values(%u, '%s',%d)",
                      characNo, db->blob_to_str(0, zipped, zippedLen),
                      WongWork::CEventMgr::GetCoinCharacCreate());
    }
    ok = db->exec(true);
    if (!ok)
    {
        return false;
    }

    db->set_query(
        "inSert into charac_inven_expand(charac_no,cargo,cargo_capacity,jewel) values(%u,'',0,'')",
        characNo);
    ok = db->exec(true);
    if (!ok)
    {
        return false;
    }
    return true;
}