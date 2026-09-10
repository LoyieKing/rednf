bool DisPatcher_DebugCommand::_debugCommandSetUserDefine(CUser* user,
                                                         PacketBuf& packet)
{
    // P190：ORIG sub $0x140 撑帧（无其它局部）；保留等价栈预留。
    char framePad[0x140];   // P190 帧尺寸撑到 0x140
    (void)framePad;
    // ORIG 0x085908d0：成员函数 ABI 下 `cmpl $0,0xc(%ebp)` 即 user 参数；
    // `je <mov$1>` → user==0 返 1（P56 双分支 mov$0/mov$1 形态）。
    if (user != 0)
        return false;
    return true;
