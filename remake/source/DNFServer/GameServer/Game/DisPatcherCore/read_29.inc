int DisPatcher_Lottery::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)

{
    return 0; // ORIG 0x08233276：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
