// ----------------------------------------------------------------------------
// DB_AutoPunishedSecondUserData::makeRequest(uint accId, ull gold, uint uid, uint who)
// ORIG @ 0x0843a790（报告 docs/class_func_reports/DB_AutoPunishedSecondUserData/
// makeRequest.md）：Acquire("DBThread.cpp", 0x8912) → guard(stream,true) →
//   << 0x1ca | << (uint)uid(arg@0x14)
// → GetInBuffer<SIG_SECOND_USER_DATA>()（0x14 字节）清零后填：
//   +0x00 = accId, +0x04..0x0b = gold(8B), +0x0c = uid, +0x10 = who
// → put(DB_Q)。
void DB_AutoPunishedSecondUserData::makeRequest(unsigned int accId, unsigned long long gold,
