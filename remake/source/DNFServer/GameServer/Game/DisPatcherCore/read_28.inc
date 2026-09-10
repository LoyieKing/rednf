int DisPatcher_DisJointItem::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0; // ORIG 0x0823339c：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
