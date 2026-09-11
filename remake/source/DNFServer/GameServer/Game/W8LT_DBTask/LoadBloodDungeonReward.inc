// ============================================================================
// LoadBloodDungeonReward @ 0x08437f58
// GetDBHandle(DB_HANDLE_2, 0) → today=getDailyInitializeTime(6,0,0) →
// localtime_r → tm_wday==0 ? 0x7e900 : (tm_wday-1)*0x15180 周偏移；
// 查询本周行（exec!=1 返 0），n_rows==1 时 fetch 后逐列读 5 字段，
// tm 时/分/秒清零 mktime 归一 last_play_date（不一致则 enter_count=0）；
// 再查询上周行（exec!=1 返 0），n_rows==1 时读 rank/reward/reward_item_id/
// reward_gold（reward = 读值==0）。全成返 1。
// ============================================================================
int DB_LoadNoCacheData::LoadBloodDungeonReward(SIG_LOAD_NO_CACHE_DATA* sig)
