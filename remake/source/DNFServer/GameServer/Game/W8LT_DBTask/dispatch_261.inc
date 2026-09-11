// ============================================================================
// DB_InsertDungeonEntrance_hour::dispatch —— ORIG 0x084298a2
// GetDBHandle(DB_HANDLE_4,0) → >>count(int) → occDate=getCurSec()、
// channelNo=get_channel_no()。循环 count 次：>>dungeonIndex >>enterCount
// >>exerciseCount（3×int）；enterCount!=0 时按 s.size()!=0 选逗号前缀
// sprintf 拼 "(ch,from_unixtime(occ),dung,hour_enter,cnt_enter)" 入 buf
// [0x400] 再 s+=buf。s 非空时 inSert log_dungeon_entrance_hour values %s；
// exec 失败返 0；成功返 1。
// ============================================================================
// [b28d-solo-3] AE + 真 bug 修复：ORIG `mov $0x100,%edx; rep stos` 的 edx 是
// **dword 计数** = 0x400 字节（P116/P253），旧 buf[0x100] 少清 0x300 字节 ⇒
// buf[0x400]={'\0'}；帧/槽与 ORIG 全等（151=151）。ORIG 084298a2。
bool DB_InsertDungeonEntrance_hour::dispatch(int userIdx, int slot,
                                             Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    int count = 0;
    *stream >> count;
    int occDate = GlobalData::s_systemTime_.getCurSec();
    int channelNo = G_CEnvironment()->get_channel_no();
    int dungeonIndex = 0;
    int enterCount = 0;
    int exerciseCount = 0;
    // ORIG 0x084298a2：`mov $0x100,%edx; rep stos` = 0x100 dwords = 0x400 字节
    // （P116/P253：ecx 是 dword 计数，旧 buf[0x100] 少清 0x300 字节 = 真 bug）。
    char buf[0x400] = { '\0' };
    std::string s;
    for (int i = 0; i < count; ++i)
    {
        *stream >> dungeonIndex;
        *stream >> enterCount;
        *stream >> exerciseCount;
        if (enterCount == 0)
            continue;
        if (s.size() != 0)
        {
            sprintf(buf, ",(%d,from_unixtime(%d),%d,%d,%d)", channelNo,
                    occDate, dungeonIndex, enterCount, exerciseCount);
        }
        else
        {
            sprintf(buf, "(%d,from_unixtime(%d),%d,%d,%d)", channelNo,
                    occDate, dungeonIndex, enterCount, exerciseCount);
        }
        s += buf;
    }
    if (s.size() != 0)
    {
        db->set_query(
            "inSert into log_dungeon_entrance_hour(channel_no,occ_date,dungeon_index,hour_enter_count, cnt_enter_count) values %s",
            s.c_str());
        if (!db->exec(true))
            return 0;
    }
    return 1;
}