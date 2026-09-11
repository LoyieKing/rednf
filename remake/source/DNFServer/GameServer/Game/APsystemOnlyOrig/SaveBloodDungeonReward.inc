// ============================================================================
// SaveBloodDungeonReward @ 0x084374f8
// GetDBHandle(DB_HANDLE_2, 0) → today=getDailyInitializeTime(6,0,0) →
// localtime_r → tm_wday==0 ? 0x7e900 : (tm_wday-1)*0x15180 周偏移；
// sig+0x1c(reward 标志)!=0 → update reward=1（week_occ_date=周起始-0x93a80），
// exec!=1 返 0；sig+0x14(enter_count)!=0 → update 5 字段 + last_play_date=
// sig+0x10（exec!=1 返 0），affected==0 时 insert 全行（exec!=1 返 0）。
// 全成返 1。
// ============================================================================
int DB_SaveBloodDungeon::SaveBloodDungeonReward(SIG_BLOOD_DUNGEON* sig)
