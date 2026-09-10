PacketGuard::~PacketGuard()
{
    // ORIG 0x0858de70：m_pool==0 且 m_external!=1 → 打池类型错误日志（get()+
    // get_index+pthread_self）；否则 m_pool->Free(get())。基类 dtor 收尾。
    if (m_pool)   // ORIG 正条件：test %eax; je <log>，Free 落 fall-through
    {
        m_pool->Free(m_packetBuf);
    }
    else
    {
        if (m_external != 1)   // ORIG this[8]（bool 字节, xor$1/test 形）
        {
            LogManager::logFormat(
                1, "packet_buf.cpp", __PRETTY_FUNCTION__, 0x3f8,
                "[PacketPool Free err - pool_type_err] pool_type(%08x), packet_idx(%d)",
                pthread_self(), m_packetBuf->get_index());
        }
    }
    // [b24ab] 基类 ~InterfacePacketBuf 由编译器自动收尾（ORIG +0x9e call 0858e29a，
    // 非 AcquirePacketBuf(0)→ 后者多 this,0 两参数 + 多余 call）
