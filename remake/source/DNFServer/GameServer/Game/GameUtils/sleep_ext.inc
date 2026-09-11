// ---- sleep_ext（ORIG _Z9sleep_extii，0x86b1b48，0x3f 字节）----
// ORIG：构造 {sec, usec} timeval 后 select(0,0,0,0,&tv)（sleep 的 select 实现）。
void sleep_ext(int sec, int usec)
