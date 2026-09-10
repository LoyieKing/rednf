void StatisticsToObserveCrash::unlock() { m_mutex.unlock(); }

// ORIG 0x841ba96：G_GameWorld()->getStatisticsPvP() → lock →
// GetDBHandle(DB_HANDLE_4,0) → 遍历 m_roomResults（PvP_Key{a,b} →
// PvP_Room{count,total}）：
//   每项先 inSert pvp_status(occ_date,room_number,play_type,no_participants,
//   occ_count) values(cast(now() as date),key.a,key.b,room.total,room.count)；
//   exec!=1 → upDate set occ_count=occ_count+room.count,
//   no_participants=no_participants+room.total where occ_date=cast(now() as
//   date) and room_number=key.a and play_type=key.b；仍失败 →
//   cMyTrace(0x4ed8,5) "upDate pvp_status failed at %d"(key.a)；
//   成功（insert 或 update）→ cMyTrace(0x4edc,5) "pvp_status : %d ROOM %d
//   TYPE %d Players %d Count "(key.a,key.b,room.total,room.count)。
// 循环结束 unlock → return 1。
// [b28d-4] DIFF：P210/N82：帧继承差（ORIG push/pop callee-saved vs 4.4 leave/槽化）；ORIG 0841ba96
// [L4-r1] 2 处 cMyTrace 改 P227 全表达式临时链 ⇒ **DIFF→NEAR**（ndiff 112→12）；
//   余 12 行 = 迭代器槽阶梯/寄存器角色（N7/N86 族）。
