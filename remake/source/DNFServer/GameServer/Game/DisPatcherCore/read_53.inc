int DisPatcher_DiePvP::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    // ORIG 0x08232b6a: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
