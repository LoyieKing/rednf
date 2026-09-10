bool DB_UpdateRevengeData::dispatch(int /*userIdx*/, int /*slot*/,
                                    Stream* stream)
{
    unsigned int characNo = 0;   // -0x3c
    unsigned char attack = 0;    // -0x3d
    unsigned char revenge = 0;   // -0x3e
    *stream >> characNo;
    *stream >> attack;
    *stream >> revenge;
    if (characNo == 0)
        return 0;
    time_t t = GlobalData::s_systemTime_.getCurSec();      // -0x44
    struct tm* p = localtime(&t);                          // -0x18
    // 技巧21：dead 前置声明占 -0x14（ORIG mktime 结果=-0x14、hourLimit
    // 后声明占更近 -0x10；0x0843f34c `mov %eax,-0x10(%ebp)`+`cmp
    // -0x10(%ebp),%eax` 实证）。
    time_t dead;
    int hourLimit = G_CEnvironment()->m_serverEnvir.m_fatigueTime;  // -0x10
    if (p->tm_hour < hourLimit)
    {
        p = 0;
        t -= 0x15180;
        p = localtime(&t);
    }
    dead = mktime(p);                                      // -0x14（ORIG 弃用）
    // ORIG +0x0c0 显式 call memset(date,0,12)（P4 反例：`= {'\\0'}` 会内联
    // 展开 movl×3，与 ORIG 库调用形态不符）。
    char date[12];                                         // -0x50
    memset(date, 0, 12);
    sprintf(date, "%04d-%02d-%02d", p->tm_year + 0x76c, p->tm_mon + 1,
            p->tm_mday);
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3,
                                                  E_SERVER_GROUP_NONE);
    if (db == 0)
        return 0;
    db->set_query(
        "upDate village_attack_dungeon set attack_count = %u, revenge_dungeon = %u where occ_date = '%s' and charac_no = %u",
        (unsigned int)attack, (unsigned int)revenge, date, characNo);
    if (db->exec(true) != 1)
    {
        // 技巧3：临时链式（具名 trace 块对象致 4.4 帧底外推，先例
        // DB_LoadBloodBestRecord 闭合 / DB_LoadPowerWarStatueInfo::dispatch）。
        cMyTrace(__PRETTY_FUNCTION__, 0x98fc, 5)(
            "village_attack_dungeon upDate exec fail");
        return 0;
    }
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into village_attack_dungeon(occ_date, charac_no, attack_count, revenge_dungeon) values('%s', %u, %u, %u)",
            date, characNo, (unsigned int)attack, (unsigned int)revenge);
        if (db->exec(true) != 1)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x9907, 5)(
                "event_integration_charac inSert exec fail");
            return 0;
        }
    }
    return 1;
