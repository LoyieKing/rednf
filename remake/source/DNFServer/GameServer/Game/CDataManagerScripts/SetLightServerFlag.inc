void SetLightServerFlag(bool flag)
{
    bool* lightFlag = GetLightServerFlag();
    // ORIG 0x8a8cba8 帧 0x28、arg-spill -0x1c、指针槽 -0xc；4.4 需 12B dead pad
    // 撑帧使 arg spill 恰落 -0x1c（probe 实证：(void)pad 须在末次使用之后，
    // 否则 4.4 把死数组槽折叠）。尾读 movzbl -0x1c,%edx @0x8a8cbbc
    char pad[12];
    *lightFlag = flag;
    (void)pad;
