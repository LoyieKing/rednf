// ORIG 0x810b0ac（32 字节）：基类实现为纯 return 0 —— 参数仅存栈不使用。
bool CEventEntity::send_init_data(CUser*, PacketGuard&, short,
                                  unsigned char, bool)
{
    return false;
}