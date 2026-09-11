// ---- DB_InsertUnlimitSupportLog::makeRequest ORIG @ 0x0842ebd0 ----
// ORIG 首指令 cmpl $0x946501,param_1; jbe → uid <= 0x946501 直接返回。
// GetInBuffer<SIG_INSERT_UNLIMIT_SUPPORT_LOG>：+0x00=uid、+0x04=serverGroup
// （G_CEnvironment()+0x378 == m_serverEnvir.m_serverGroup，具名链见 CEnvironment.h）、
// +0x08=characNo、+0x0c strncpy(name,0x1d)（ORIG 无 memset）。投 DB_Q（实参 2）。
void DB_InsertUnlimitSupportLog::makeRequest(unsigned int uid, unsigned int characNo,
