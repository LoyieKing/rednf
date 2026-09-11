// ORIG 0x08106bfc（W 同址 C1/C2）：tagPacketHeader 构造 + 三 setter。
tagCS_UPDATE_CHANNEL_INFO::tagCS_UPDATE_CHANNEL_INFO()
{
    // 基类 tagPacketHeader 自动默认构造（ORIG 0x08106bfc 仅一次构造调用；
    // 显式 tagPacketHeader::tagPacketHeader(); 会让 g++ 多构造一个临时对象）
    setCategory(1);
    setPacketID(2);
    setSize(0x33);
}