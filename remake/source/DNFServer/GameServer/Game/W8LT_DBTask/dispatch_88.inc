// ============================================================================
// DB_UpdatePvPGrade::dispatch —— ORIG 0x08428b26
// ============================================================================
// ORIG 0x8428b26：GetDBHandle(DB_HANDLE_2,0) → seLect pvp_grade_expand 排行
// （pvp_grade>=getPvPRankRateGrade()，limit 1500）；exec 失败或 0 行返 0。
// 昨日 1:00 的 mktime 基准（limitTime=cur-0x15180）筛 last_play_time；
// 行满足「pvp_grade>=getPvPExpLossGrade() && last_play<=limitTime &&
// pvp_point>refPoint(GetPvpPoint(getPvPExpLossGrade()))」：
//   pvp_point > getPvPExpLossExp()+refPoint → lossList（pvp_point-=expLoss）
//   否则 → resetList（pvp_point=refPoint）；pvp_point=max(refPoint, 原- expLoss)。
// 全部行入 rows（stPvPTempTable_t，charac_no）。
// 然后按 getPvPRankRate() 比率（(rate[i]*rowCount+500)/1000，单调钳制 ≥1，
// rowCount!=0 && rate[3]==0 → rate[4]=1）把 rows 分级入 gradeList[5]，
// 逐级 upDate pvp_result（pvp_point 减损/置 refPoint；grade_ext=5-g）。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 08428b26
bool DB_UpdatePvPGrade::dispatch(int userIdx, int slot, Stream* stream)
