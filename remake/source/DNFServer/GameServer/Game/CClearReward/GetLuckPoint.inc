int CLuckPoint::GetLuckPoint()
{
    // [b28c-lane17] ORIG 0x085509e7：call empty 后**无** `xor $1`，直接
    // `test %al; jne 成功块` ⇒ 源形是「empty 则跳过」（负极守卫 + 空 then），
    // 非 `if (empty()) return 0;`（后者 4.4 产 xor+je 反极性）；
    // 尾部分支失败路径直落 `mov $0`（return 0），故成功分支补显式 return。
    // [b28c-lane17] ORIG 0x085509e7：`call empty; xor $1; test %al; jne 体`
    // + 尾 `mov $0` 岛 ⇒ 取反守卫 `if (!empty()) { 体 } return 0;`
    // （fresh 实测：本形 54=54 AE；裸 `if(empty()) return 0` 与
    //  `if (empty()) {体}` 包裹两候选均缺 xor 段且 54/53 DIFF）。
    if (!m_users.empty())
    {
    int sum = 0;
    for (size_t i = 0; i < m_users.size(); ++i)
    {
        if (m_users[i] != 0)
            sum += reinterpret_cast<CUserCharacInfo*>(m_users[i])->GetCurCharacLuckPoint();
    }
    // ORIG 0x085509da：divl（无符号除法，int/size_t 提升）；强制 int 会生成 idivl。
    return sum / m_users.size();
    }
    return 0;   // 空集共用尾块（ORIG +0x9f mov $0,%eax）
