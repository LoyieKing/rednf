// [P8S6 2026-09-11] N104 推翻（EH 段可复现）：glibc `memmove` 声明带 `__THROW`(throw())
//   ⇒ 4.4 判 try 块不可抛、整段消除 catch 与 landing pad；ORIG 0x084902a4 起有完整
//   `__cxa_begin_catch/__cxa_end_catch/_Unwind_Resume` 链 + 串 @0x8c78224
//   `[PARSING EXCEPTION] memmove : parsinglength = %d`（我方源码本就有该 catch，
//   只是被删）。块作用域 asm 别名 GCC 拒绝（探针 /tmp/p8probe/eh2.cpp 实证），
//   故按 Dispatchers.cpp:4624 先例在本 include 位置声明**同符号**别名（无 exception
//   spec ⇒ 可抛）：调用点 reloc 仍为 memmove、参数传递逐条同 ORIG（探针 eh.o f1/f2
//   对照：f1 无 EH、f2 完整 EH+ebx 共享出口）。
extern "C" void* p8_memmove_eh(void*, const void*, unsigned int) __asm__("memmove");
bool CSession::Parsing(int recvLen)
