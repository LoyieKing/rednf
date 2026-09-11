// ============================================================================
// DB_CrashDownReport::dispatch —— ORIG 0x0841abc6（报告 dispatch.md 全文）。
// s_systemTime_.getCurSec → localtime_r → tm_sec=0 → mktime（今晨零点，
// 全函数共享）→ GetDBHandle(DB_HANDLE_4,0) →
// getStatisticsToObserveCrash()->getAbnormalDownUserCount() → upDate
// client_down set occ_count=occ_count+%d where occ_date=cast(now() as date)
// （count）；exec!=1 → cMyTrace(0x4d5c,5) "upDate client_down failed"；
// 随后 cMyTrace(0x4d5f,0) "abnormal down user : %d"（无条件）。
// lock → 遍历 m_userLastCrashDungeonId（键=地牢,值=次数）：
//   inSert crash_down_by_dungeon(occ_time,dungeon_index,down_count)
//   values(from_unixtime(%d),%d,%d)（dayStart,dungeon,down）；exec==1 或
//   upDate 成功（实参序照 ORIG 汇编：(dayStart,down,dungeon)）→
//   cMyTrace(0x4d7d,0) "abnormal down user by dungeon : %d DUNGEON %d"
//   （dungeon,down）。
// 再遍历 m_userLastCrashPacketId：inSert crash_down_by_packet
//   (occ_date,packet_index,down_count) values(cast(now() as date),%d,%d)
//   （packet,down）；失败转 upDate（packet,down）；成功 →
//   cMyTrace(0x4d98,0) "abnormal down user by packet : %d PACKET %d"。
// 再遍历 m_userLastCrashPlayStatus（do-while 形态）：inSert
//   crash_down_by_playstatus(occ_date,play_status,down_count)
//   values(cast(now() as date),%d,%d)（play,down）；失败转 upDate
//   （play,down）；成功 → cMyTrace(0x4db2,0)
//   "abnormal down user by playerstatus : %d PLAYERSTATUS %d"。
// 三循环结束 unlock → reset → 返 1。
// ============================================================================
// [b28d-4] DIFF：P222：块序/极性（ORIG je 落点 vs 4.4 jne）；ORIG 0841abc6
bool DB_CrashDownReport::dispatch(int userIdx, int slot, Stream* stream)
