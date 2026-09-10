void generateSecurityCardQuestion(char* out);

// ============================================================================
// 1. DB_SaveConditionEventInfo::dispatch —— ORIG 0x08437930（dispatch.md）：
//    GetDBHandle(2,0) → GetOutBuffer<SIG_SAVE_CONDITION_EVENT>（+0 uint charac_no /
//    +4 short current_step / +6 short reward_step）→ set_query(upDate ...) →
//    exec(true) 失败返 0；getAffectedRowCount()==0 时 set_query(inSert ...) →
//    exec(true) 失败返 0；返 1。字段访问经 SIG 直读（ORIG movzwl/movswl/cwtl
//    符号扩展形态，P9 语义）。
// ============================================================================
