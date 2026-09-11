int Dispatcher_BuyItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    // Fix6：ORIG 空壳（mov $0x0,%eax; ret）；get_binary 在 read_packet 中。
    (void)packet; (void)msg;
    return 0;
}