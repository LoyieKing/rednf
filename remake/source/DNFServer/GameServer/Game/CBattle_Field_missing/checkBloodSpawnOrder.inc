// ---------------------------------------------------------------------------
// CBattle_Field::checkBloodSpawnOrder(short) —— ORIG T 0x08306e32：
//   小轮序在 [0,size) 且前 smallRound 个阶段均已清除（m_bloodPhaseClear 前段全 true）。
// ---------------------------------------------------------------------------
bool CBattle_Field::checkBloodSpawnOrder(short smallRound)
{
    // [b28d-39] ORIG 0x08306e32：先 `(short)size() < smallRound`（16 位 setl），
    // 再 empty()；具名 `int n=(int)smallRound`（-0x10）+ `int i`（-0xc）的
    // `i<n` 有符号循环。负数 smallRound 在非空时返 1（原码语义，照抄）。
    if ((short)m_bloodPhaseClear.size() < smallRound)
        return 0;
    if (m_bloodPhaseClear.empty())
        return 0;
    int n = (int)smallRound;
    for (int i = 0; i < n; ++i)
    {
        if (!m_bloodPhaseClear[i])
            return 0;
    }
    return 1;
}