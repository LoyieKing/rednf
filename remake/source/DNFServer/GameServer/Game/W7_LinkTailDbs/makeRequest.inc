void DB_UpdateLimitItemUsage::makeRequest(int, int, int, int, long)
{
    // ORIG 空体（0x843f030..0x843f037，7 字节 ret 函数）
    // ORIG 带 outgoing 帧 sub $0x10（4.1 空转发残留形）；P31 dead local 复现帧深。
    char framePad[0x10];
    (void)framePad;
