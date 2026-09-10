void CAssaulter::SetUserConnectP2P(char* ip, int len)   // ORIG T 0x82e6a82
{
    // ORIG 0x82e6a96 call _ZSt3minIiERKT_S2_S2_（(%esp)=&len，0x4esp=&.rodata 8
    // @0x8c1c344 ⇒ static const int 取地址），`mov (%eax),%eax` 解引用返回引用。
    static const int kIpMax = 8;
    memcpy(m_connectIp, ip, std::min(len, kIpMax));
