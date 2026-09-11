// ---- DB_InsertArchieveEventLog::makeRequest ORIG @ 0x0842e856 ----
// stream: opcode 0x117 | -1 | GetInBuffer<SIG_INSERT_ACHIEVE_EVENT>：
//   +0x00=accId、+0x04=characNo、+0x08=type。投 LOGDB_Q（ORIG put 第 2 实参 4）。
// SIG 结构在 SigTypes.h 为 0x100 占位缓冲，按报告偏移以函数内命名视图字段化
// （成员访问折叠出与 ORIG 相同的 mov %edx,0xN(%eax) 存储形态）。
void DB_InsertArchieveEventLog::makeRequest(
