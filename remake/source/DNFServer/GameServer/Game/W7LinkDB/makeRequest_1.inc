// ----------------------------------------------------------------------------
// DB_InsertAutoPunishFirstUser::makeRequest(uint accId, const char* ip,
//     uint a3, uint a4, uint a5, uint a6, ushort a7, uint a8)
// ORIG @ 0x0842dd7c（报告 DB_InsertAutoPunishFirstUser/makeRequest.md）：
// Acquire("DBThread.cpp", 0x723e) → << 0x10a | << -1 →
// GetInBuffer<SIG_INSERT_AUTO_PUNISH_FIRST_USER>()（0x2c 字节，无 memset）填：
//   +0x00 = accId；strncpy(+0x04, ip, 0x10)；+0x14..+0x20 = a3..a6；
void DB_InsertAutoPunishFirstUser::makeRequest(unsigned int accId, char const* ip,
