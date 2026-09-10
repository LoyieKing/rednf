int DisPatcher_RecoverStamina::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)

{
    return 0; // ORIG 0x081c3ea4：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
