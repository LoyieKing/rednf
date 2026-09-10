int DisPatcher_ExtendAvatar::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/)

{
    return 0; // ORIG 0x081eddec：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
