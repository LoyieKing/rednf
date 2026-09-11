// ============================================================================
// DB_UpdateRevengeData::dispatch —— ORIG 0x0843f34c（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x843f34c：解包 characNo(uint)/attack(uchar)/revenge(uchar)；
// characNo==0 返 0。getCurSec → localtime；tm_hour 早于
// G_CEnvironment+0x37c（m_serverEnvir.m_fatigueTime）则回退 86400 秒重取；
// mktime 结果弃用（P31 死局部）。date[12] 清零 0xc 后 sprintf
// "%04d-%02d-%02d"（tm_year+0x76c/tm_mon+1/tm_mday）。GetDBHandle
// (DB_HANDLE_3,0)，null 返 0。upDate village_attack_dungeon set
// attack_count=%u, revenge_dungeon=%u where occ_date='%s' and charac_no=%u；
// exec 失败 cMyTrace(0x98fc,5)("village_attack_dungeon upDate exec fail") 返 0；
// getAffectedRowCount()==0 时补 inSert（occ_date,charac_no,attack_count,
// revenge_dungeon），insert exec 失败 cMyTrace(0x9907,5)
// ("event_integration_charac inSert exec fail") 返 0；全绿返 1。
// ============================================================================
bool DB_UpdateRevengeData::dispatch(int /*userIdx*/, int /*slot*/,
