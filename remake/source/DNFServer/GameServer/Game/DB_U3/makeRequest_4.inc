// ---- DB_ReqSaveHackUserPunish::makeRequest ORIG @ 0x08425494 ----
// GetInBuffer<SIG_SAVE_HACK_USER_PUNISH>：+0x00=uid、+0x04=type、+0x08 ip：
//   NULL → memcpy "000.000.000.000"（0x10 含 NUL）；否则 strncpy(ip,0x10)。
//   两分支 ORIG 形态不同（add 直算 vs lea 经 edx），按原表达式转写。投 DB_Q。
void DB_ReqSaveHackUserPunish::makeRequest(unsigned int uid, unsigned int type,
