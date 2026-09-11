// ORIG 0x08434d36：inventory db(DB_HANDLE_3) 读 equipslot blob →
// get_binary → uncompress_zip 解压到 items+0x262（失败/超限抛 unsigned int
// 0x7f1c/0x7f1e，catch 后 logFormat(0x7f29) 返 0）；解压后大小须为
// 0x3d 倍数或 0；info+0x7c=解压区 dword；再查 user_items（expire_date>now()
// and stat=0 limit 115）按 slot(0..9) 填 equipment；clear_avatar_id 非 0 时
// 再按 ui_id 二次查 it_id 覆盖对应槽。
// [b28d-4] DIFF：N1/N107：ORIG lea+add 两步寻址 vs 4.4 折单条；ORIG 08434d36
bool DB_LoadPowerWarStatueInfo::GetAvatarInfo(
    SIG_LOAD_POWER_WAR_STATUE_INFO* sig, unsigned int characNo)
{
    static const int handleIdx = 3;
    char buf[0x37a] = { '\0' };          // ORIG rep stos 清零（P4）
    unsigned int originalSize = 0x37a;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, E_SERVER_GROUP_NONE);
    db->set_query("seLect equipslot from inventory where charac_no=%u", characNo);
    if (!db->exec(true) || !db->fetch())
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x7f09, 5);
        trace("DB_LoadPowerWarStatueInfo::GetAvatarInfo, exec() or fetch() ERROR charac_no=%u",
              characNo);
        return 0;
    }
    int nItems = 0x73;                   // ORIG 死局部（-0x2c 只写不读，P31）
    Inven_Item items[22];                // ORIG 构造 0x15→-1 共 22 次（P34）
    int rows[0x73][3];                   // slot/it_id/clear_avatar_id × 115
    memset(rows, 0, sizeof(rows));
    int dbSize = db->get_binary_length(0);
    if (!db->get_binary(0, buf, 0x37a))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x7f15, 5);
        trace("DB_LoadPowerWarStatueInfo::GetAvatarInfo, get_binary(0) ERROR charac_no=%u",
              characNo);
        return 0;
    }
    try
    {
        if (dbSize > (int)originalSize)
            throw (unsigned int)0x7f1c;
        if (!uncompress_zip((char*)&items[10], originalSize, buf, dbSize))
            throw (unsigned int)0x7f1e;
    }
    catch (unsigned int e)
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x7f29,
            "Uncompress failed. charac_no(%d)(%d)", characNo, e);
        return 0;
    }
    if (originalSize % 0x3d != 0 && originalSize != 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x7f23, 5);
        trace("incorrect equipment size charac_no(%d), equipment, original_size(%d), db_size(%d)\n",
              characNo, 0x53e, dbSize);
        return 0;
    }
    SIG_LOAD_POWER_WAR_STATUE_INFO* v = sig;
    v->m_field7c =
        reinterpret_cast<DB_EquipslotBlobView*>(&items[10])->m_dword2;
    db->set_query(
        "seLect slot,it_id,clear_avatar_id from user_items where charac_no=%u and expire_date>now() and stat=0 limit %d",
        characNo, 115);
    if (!db->exec(true))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x7f35, 5);
        trace("DB_LoadPowerWarStatueInfo::GetAvatarInfo, exec() ERROR charac_no=%u",
              characNo);
        return 0;
    }
    int nRows = db->get_n_rows();
    for (int i = 0; i < nRows; ++i)
    {
        if (!db->fetch())
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x7f3f, 5);
            trace("DB_LoadPowerWarStatueInfo::GetAvatarInfo, exec() ERROR charac_no=%u",
                  characNo);
            break;
        }
        if (!db->get_int(0, rows[i][0]))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x7f45, 5);
            trace("DB_LoadPowerWarStatueInfo::GetAvatarInfo, get_int(0) ERROR charac_no=%u",
                  characNo);
            return 0;
        }
        if (!db->get_int(1, rows[i][1]))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x7f4b, 5);
            trace("DB_LoadPowerWarStatueInfo::GetAvatarInfo, get_int(1) ERROR charac_no=%u",
                  characNo);
            return 0;
        }
        if (!db->get_int(2, rows[i][2]))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x7f51, 5);
            trace("DB_LoadPowerWarStatueInfo::GetAvatarInfo, get_int(2) ERROR charac_no=%u",
                  characNo);
            return 0;
        }
        if (rows[i][0] >= 0 && rows[i][0] <= 9)
            v->m_equipment[rows[i][0]] = rows[i][1];
    }
    for (int j = 0; j < nRows; ++j)
    {
        if (rows[j][2] != 0)
        {
            db->set_query("seLect it_id from user_items where ui_id=%d", rows[j][2]);
            if (db->exec(true) && db->fetch())
                db->get_int(0, v->m_equipment[rows[j][0]]);
        }
    }
    return 1;
}