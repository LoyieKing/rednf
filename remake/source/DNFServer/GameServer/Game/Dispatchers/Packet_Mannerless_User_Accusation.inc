Packet_Mannerless_User_Accusation::Packet_Mannerless_User_Accusation()
{
    // [b28c-34] P0 清绑：原 `register void* self asm("eax") = this;` 已删（N96）；
    // 裸偏移 memset 改具名 m_chatMsg（§4.3）。ORIG 0x0822c1f6 this 装载先于常量 spill。
    ::new (static_cast<void*>(this)) PacketHeader(0x1b66, 0x15f);   // 0x0822c202
    m_characNo = -1;                                           // 0x0822c20a：+0xa = -1
    m_type = 0;                                                // 0x0822c214：+0xe = 0
    m_serverGroup = 4;                                         // 0x0822c21e：+0x12 = 4
    m_accIdHi = 0;                                             // 0x0822c228：+0x1e = 0
    memset(m_name, 0, 0x1e);                                   // 0x0822c235：+0x22[30] 清零
    memset(m_targetName, 0, 0x1e);                             // 0x0822c24a：+0x40[30] 清零（NEW 曾漏）
    memset(m_chatMsg, 0, 0x100);                               // 0x0822c26b：+0x5e[0x100]
    m_result = 0;                                              // ORIG 0x0822c: movb $0,0x15e(%eax) 单级
