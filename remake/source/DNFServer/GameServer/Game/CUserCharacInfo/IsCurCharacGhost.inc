bool CUserCharacInfo::IsCurCharacGhost()
{
    // ORIG：true 路单一 `movzbl 0xe66(%eax),%eax` 直返（uchar 返回 = load 即
    // 终值，无 int promote 二段 movzbl %al；bool 声明反产 test;je;mov$1 规范化）
    // ⇒ 返回类型 unsigned char（技巧 33，mangled 无返回类型）。
    if (m_selected)
        return CUR->m_ghost;
    return 0;
