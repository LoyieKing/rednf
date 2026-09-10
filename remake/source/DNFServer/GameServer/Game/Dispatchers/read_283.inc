int Dispatcher_Exchange_Random_Item_Reward::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x：槽序（4.4 先声明→远槽，probe slots4 实证）：
    // item=-0x18（首声明）、count=-0x15、data ptr=-0x10、i=-0xc（末声明）。
    // data 指针槽首 store 0x08：mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)，
    // 后续访问一律槽重载（mov -0x10(%ebp),%eax）。初始化落点=声明位置（技13）：
    // ORIG `movb $0,-0x15` 在第一 LineFunc 后、`movw $0,-0x18` 在循环前。
    MSG_EXCHANGE_RANDOM_ITEM_REWARD* data =
        reinterpret_cast<MSG_EXCHANGE_RANDOM_ITEM_REWARD*>(&msg);
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(
            reinterpret_cast<char*>(data) + 0xe)))
    {
        return LineFunc(0x4fc8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned char count = 0;
    if (!packet.get_byte(count))
    {
        return LineFunc(0x4fcc,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned short item = 0;
    // ORIG COND：movzbl count; cmp i; seta（unsigned 比较，技巧：双方同 unsignedness）。
    for (unsigned int i = 0; count > i; ++i)
    {
        if (!packet.get_short(item))
        {
            return LineFunc(0x4fd2,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        data->m_rewardItems.push_back(static_cast<int>(item));
        item = 0;                                   // ORIG 尾重置 movw $0,-0x18(%ebp)
    }
    return 0;
