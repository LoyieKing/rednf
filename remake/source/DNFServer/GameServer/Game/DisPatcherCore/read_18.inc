// dispatch_sig 真实实现见文件尾第二批段（ORIG T 0x081f3540）。
int DisPatcher_SendMess::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)

{
    return 0; // ORIG 0x08232f66：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
