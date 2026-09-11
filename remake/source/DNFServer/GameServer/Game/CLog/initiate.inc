// ORIG initiate 0x0854e88c：
//   stat(name) 失败 → mkdir(name, 0x1fd)；仍失败 printf 后 return。
//   +0x250=flag、+0x254=1；m_logFile(+0x258) ← path（空则 "Log" 4 字节）；
//   8 个 TimeLog_@+0x210..+0x248 逐个 setTime；name 为空 → 句柄置 stdout
//   （0xf0/0xf4/0xf8/0xfc/0x100/0x108/0x10c，无 0x104——ORIG 照抄）；
//   否则 strcpy m_initFile(+0x110) 并 8 次 openLogFile（history 用 +0x254 flag）。
void CLog::initiate(char* name, char* path, int flag)
