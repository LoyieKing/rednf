bool DB_StatPVPPlay2::dispatch(int userIdx, int slot, Stream* stream)
{
    // P157/P183：ORIG 三处独立 `G_GameWorld()->getStatisticsPvP()`（p167 差集
    // -3/-3；0x0841ba96），禁指针缓存。
    G_GameWorld()->getStatisticsPvP()->lock();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    std::map<StatisticsPvP::PvP_Key, StatisticsPvP::PvP_Room>::iterator it =
        G_GameWorld()->getStatisticsPvP()->m_roomResults.begin();
    std::map<StatisticsPvP::PvP_Key, StatisticsPvP::PvP_Room>::iterator end =
        G_GameWorld()->getStatisticsPvP()->m_roomResults.end();
    // [SweepIntervalSet 真 bug] 原 `while (it != end) { … continue; … ++it; }` 的
    // continue 跳到条件处、跳过 ++it ⇒ update 也失败时 it 不变 = 死循环。
    // ORIG 0x0841bca0 单一 operator++ 调用点在循环尾（continue 必须经过它）⇒
    // 源形为 `for (; it != end; ++it)`（4.1 布局：入口 jmp 条件 / body / ++it /
    // 条件 jne body，实证 0x0841bb18/0x0841bc9a/0x0841bcb9）。
    // 另：ORIG 0x0841bb89 / 0x0841bbe8 两处 exec 结果各占具名槽（-0x1a/-0x19，
    // `mov %al,槽; movzbl 槽,%eax; xor $1`）⇒ 两个具名 bool 局部（P0/P102）。
    for (; it != end; ++it)
    {
        // P167：ORIG 用 `operator*`（deEv）×2，我方 `it->` 选中 ptEv ×2 ⇒
        // 写 `(*it).first/second`。
        const StatisticsPvP::PvP_Key& key = (*it).first;
        const StatisticsPvP::PvP_Room& room = (*it).second;
        db->set_query(
            "inSert into pvp_status(occ_date,room_number,play_type,no_participants,occ_count) values(cast(now() as date),%d,%d,%d,%d)",
            key.m_a, key.m_b, room.m_resultSum, room.m_roomCount);
        bool inserted = db->exec(true);
        if (!inserted)
        {
            db->set_query(
                "upDate pvp_status set occ_count = occ_count + %d , no_participants = no_participants + %d where occ_date = cast(now() as date) and room_number = %d and play_type = %d",
                room.m_roomCount, room.m_resultSum, key.m_a, key.m_b);
            bool updated = db->exec(true);
            if (!updated)
            {
                // [L4-r1] P227 全表达式临时链（ORIG ctor 前取 varargs + 跨 ctor 保活）。
                cMyTrace(__PRETTY_FUNCTION__, 0x4ed8, 5)(
                    "upDate pvp_status failed at %d", key.m_a);
                continue;  // for 的 increment 处 = ORIG 单一 operator++ 调用点
            }
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x4edc, 5)(
            "pvp_status : %d ROOM %d TYPE %d Players %d Count ",
            key.m_a, key.m_b, room.m_resultSum, room.m_roomCount);
    }
    G_GameWorld()->getStatisticsPvP()->unlock();
    return 1;
