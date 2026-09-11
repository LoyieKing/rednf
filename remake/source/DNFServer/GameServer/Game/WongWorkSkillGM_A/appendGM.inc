// ORIG T 0x081093ee appendGM(unsigned,unsigned)：恒返回 0。
int CGMAccounts::appendGM(unsigned int /*mid*/, unsigned int /*type*/)
{
    int dead_frame;                        // ORIG sub $0x10,%esp + leave（0x10 无读写帧）
    (void)&dead_frame;                     // P31 dead local 帧差对齐（探针实证取址保槽）
    return 0;
}