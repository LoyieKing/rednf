int CAssaultPlace::_GetUserIndex(CUser* user, bool strict)
{
    // ORIG T 0x82e849a：找不到时 strict=true 抛 ENUM_ERROR(0x15)，否则返回 -1。
    // call Empty 后 test %al;jne skip（0x82e84bb 区）；比较 cmp+sete+test（技巧22）。
    for (int i = 0; i <= 7; ++i)
    {
        if (m_assaulters[i].Empty())
            continue;
        // N96 清绑：call 结果与形参直比（P197④ 出 sete 物化）。
        if (m_assaulters[i].GetUser() == user)
            return i;
    }
    if (strict)
        throw (ENUM_ERROR)0x15;
    return -1;
}