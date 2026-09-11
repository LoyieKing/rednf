// ORIG 0x08694252 (sz 0x15d)：
//   stPacket();
//   CPacketHelper helper(this->m_body, 0x1000, 0);
//   helper.padding((unsigned char*)&login.m_type,        1);           // +0x000 len 1
//   helper.padding((unsigned char*)&login.m_accLen,      1);           // +0x001 len 1
//   helper.padding((unsigned char*) login.m_account,
//                  (unsigned char)login.m_accLen);                     // +0x002 len accLen
//   helper.padding((unsigned char*)&login.m_charLen,     1);           // +0x101 len 1
//   helper.padding((unsigned char*) login.m_characName,
//                  (unsigned char)login.m_charLen);                    // +0x102 len charLen
//   helper.padding((unsigned char*)&login.m_ip,          4);           // +0x201 len 4
//   helper.padding((unsigned char*)&login.m_canTry,      1);           // +0x205 len 1
//   helper.padding((unsigned char*)&login.m_userIdx,     8);           // +0x206 len 8 (userIdx+accId)
//   SetLength(helper.get_offset()+1);
//   m_cmd = 0x27;
stLoginPacket::stLoginPacket(stLogin& login)
{
    CPacketHelper helper(m_body, MaxBodySize, 0);

    helper.padding(reinterpret_cast<unsigned char*>(&login.m_type), 1);
    helper.padding(reinterpret_cast<unsigned char*>(&login.m_accLen), 1);
    helper.padding(
        reinterpret_cast<unsigned char*>(login.m_account),
        static_cast<int>(static_cast<unsigned char>(login.m_accLen)));
    helper.padding(reinterpret_cast<unsigned char*>(&login.m_charLen), 1);
    helper.padding(
        reinterpret_cast<unsigned char*>(login.m_characName),
        static_cast<int>(static_cast<unsigned char>(login.m_charLen)));
    helper.padding(reinterpret_cast<unsigned char*>(&login.m_ip), 4);
    helper.padding(reinterpret_cast<unsigned char*>(&login.m_canTry), 1);
    // ORIG 一条 padding 长度 8：连续覆盖 userIdx(+0x206..+0x209)+accId(+0x20a..+0x20d)
    helper.padding(reinterpret_cast<unsigned char*>(&login.m_userIdx), 8);

    SetLength(static_cast<unsigned short>(helper.get_offset() + 1));
    m_cmd = 0x27;
}