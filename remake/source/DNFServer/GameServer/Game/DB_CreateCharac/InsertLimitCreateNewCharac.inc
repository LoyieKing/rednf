// ===========================================================================
// InsertLimitCreateNewCharac(SIG*) — 报告 @0x084016ae：新行插入 + trace。
// ===========================================================================
// b28d-solo-DB_CreateCharac verdict=AE ORIG 0x084016ae 110=110。
// 修法：P227 全表达式临时链（ORIG ctor 前右起求值 + %esi/%edi/%ebx 保活）。
bool DB_CreateCharac::InsertLimitCreateNewCharac(SIG_CREATE_CHARAC* sig)
