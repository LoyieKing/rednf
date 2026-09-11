// ============================================================================
// CLocalGlobal_DB_STATICS_BOSSTOWER::dispatch —— ORIG 0x0812825e
// GetDBHandle(DB_HANDLE_4,0) → >>keyCount（<=0 返 0）→ __GetCurrentDNFDate +
// sprintf("%04d-%02d-%02d") 日期 → 循环 keyCount 次：>>key(u32) +
// GetOutBuffer<STBossDungeonTowerLog>(+0 play_cnt/+4 success_cnt/+8 uniq_cnt/
// +0xc play_time) → UPDATE log_monstertower_layer_stat；exec 失败或受影响行数
// ==0 时改 INSERT（key 高 4 位打包 layer/monster_diff/charac_job/charac_grow/
// level，ORIG 逐字节分解）。INSERT exec 结果丢弃。恒返 1。
// ============================================================================
// [b28d-4] DIFF：P219/P248：字节字段物化/同型化差（ORIG movzbl vs 4.4 mov/shr/and）；ORIG 0812825e
bool CLocalGlobal_DB_STATICS_BOSSTOWER::dispatch(int userIdx, int slot,
