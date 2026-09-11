// ORIG T 0x085b9e20：循环 i=0..3，_checkValidUser 过滤后读 (this+0x78+i*0x18)
// 的 CUser* 与 user 比较，命中返回 i，否则 -1。
int CParty::GetUserPosition(CUser* user)
{
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i) != 1)
            continue;
        // ORIG 085b9e57-5d `lea (%ecx,%eax,1)+0x78; mov (%eax)` = m_member[i].m_pUser
        if (m_member[i].m_pUser == user)
            return i;
    }
    return -1;
}