void CUser::SetGuildInfoProxy(STGuildProxy& proxy)
{
    m_guildDB.m_fundLevel = proxy.m_level;
    // ORIG 0x0865cd20：`mov 0xc(%ebp),%eax` src 先载 + dst `mov 0x8(%ebp),%edx;
    // add $0x8d020,%edx` 两段。
    // [b28d-solo-CUserOnlyOrig P0] ORIG 无 src 槽 ⇒ 原码 src 是内联表达式
    // （旧 `void* sp = &proxy;` 生 -0xc 槽 + 回读，帧 0x28 vs ORIG 0x18）。
    memcpy(&m_guildDB, &proxy, 0x16);
    // ORIG 0x0865cd43：`mov 0xc(%ebp),%eax; movzbl 0x18(%eax),%edx; mov 0x8(%ebp),%eax;
    // mov %dl,0x8d0b5(%eax)`——uchar=uchar 同型单步零扩展直落（技巧5；Main 已落
    // CUser.h:1307 m_field95 char→uchar，同宽 sizeof/偏移不变）。
    m_guildDB.m_field95 = proxy.m_powerSide;
