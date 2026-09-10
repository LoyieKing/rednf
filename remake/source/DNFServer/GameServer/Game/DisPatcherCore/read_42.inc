int DisPatcher_GiveUpGame::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)

{
    // ORIG 0x081c40aa: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}
