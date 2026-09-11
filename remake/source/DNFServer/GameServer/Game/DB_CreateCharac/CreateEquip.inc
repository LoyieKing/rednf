// ===========================================================================
// CreateEquip — 报告 @0x084023ac
// item==NULL 直接成功；否则 zip(item, 0x2dc) upDate equipslot 列。
// ===========================================================================
// b28d-solo-DB_CreateCharac verdict=AE ORIG 0x084023ac 88=88。
// 修法：P312 `char zipped[0x37a]={0}` + 非 4 对齐槽 -0x38e 复现内联清零慢路
// + P0 bool ok 声明在 db 前（-0xd/-0xc）。
bool DB_CreateCharac::CreateEquip(unsigned int characNo, Inven_Item* item)
{
    static const int handleIdx = 3;
    if (item == NULL)
    {
        return true;
    }

    // P312/N78：`char buf[N]={0}` + 非 4 对齐槽（-0x38e）复现 4.1 memset 内联慢路
    //（对齐剥离 2B + rep stos + 尾 2B/1B）。
    char zipped[0x37a] = {0};
    unsigned int zippedLen = 0x37a;
    if (!compress_zip(zipped, zippedLen, reinterpret_cast<char*>(item), 0x2dc))
    {
        return false;
    }

    // P0：ORIG `mov %al,-0xd` 槽往返 ⇒ 原码有具名 bool 局部（P168a① 裸形）；
    // 声明在 db 之前使 4.4 给 bool 更深槽（-0x10 尾 -0xd）、db 落 -0xc（同 ORIG）。
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    db->set_query("upDate inventory set equipslot='%s' where charac_no = %u",
                  db->blob_to_str(0, zipped, zippedLen), characNo);
    ok = db->exec(true);
    if (!ok)
    {
        return false;
    }
    return true;
}