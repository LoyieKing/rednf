CUser* CHackAnalyzer::getUserP()
{
    // ORIG T 0x080f79ce（getUserP.md）：非零路径 fallthrough（回读同槽），
    // 空值返回置函数尾（与 ORIG 分支排布一致）。
    if (m_baseHackType != 0)
        return (CUser*)m_baseHackType;
    return (CUser*)0;
