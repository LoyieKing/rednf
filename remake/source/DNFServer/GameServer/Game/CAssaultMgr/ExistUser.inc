bool CAssaultPlace::ExistUser(CUser* user)
{
    // ORIG T 0x82e8366：call Empty 后 `test %al,%al; jne skip`（无 xor，
    // continue 形态）；GetUser==user 产 cmp+sete+test 三连接（技巧22）。
    for (int i = 0; i <= 7; ++i)
    {
        if (m_assaulters[i].Empty())
            continue;
        // N96 清绑：call 结果与形参直比（P197④ 出 sete 物化）。
        if (m_assaulters[i].GetUser() == user)
            return true;
    }
    return false;
}