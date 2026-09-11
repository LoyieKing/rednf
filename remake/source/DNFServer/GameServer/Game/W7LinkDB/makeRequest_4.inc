// ----------------------------------------------------------------------------
// DB_HackIPSave::makeRequest(const char* startIp, const char* rawIp, uint a3, uint a4)
// ORIG @ 0x08436ff8（报告 DB_HackIPSave/makeRequest.md）：
// Acquire("DBThread.cpp", 0x82be) → << 400(0x190) | << -1；
// 守卫：仅当 startIp==NULL 或 strlen(startIp)>7 时继续（否则直接返回不投递）。
// GetInBuffer<SIG_HACK_IP_SAVE>()（0x28 字节，无 memset）填：
//   strncpy(+0x00, startIp, 0xd)；+0x20 = a3；+0x24 = a4；
//   strncpy(+0x0d, rawIp, 0x10)。→ put(DB_Q)。
// ----------------------------------------------------------------------------
void DB_HackIPSave::makeRequest(char const* startIp, char const* rawIp,
