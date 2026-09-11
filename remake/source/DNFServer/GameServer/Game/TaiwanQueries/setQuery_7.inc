// ORIG T 0x08175224：%d 参数序 price/c6/StringToNumber(uidStr)/c8 与 ORIG 一致。
// [b28BM13 N7] 封顶：指令/槽/助记符全等，仅 5 个 callee-saved 分配整体错位
// （4.1 首位 edx vs 4.4 首位 edi）。register 绑定按 P0 全域禁用（旧 asm("edx")
// 尝试注记作废，实测亦劣化），穷举后封顶。
bool CeraPointQuery::setQuery(MySQL& db)
{
    return db.set_query(
        "call usp_point_process('%s', '%s', '%c', %u, %d, %d , %d, @out_code)",
        m_account, m_p2, static_cast<char>(m_c8), m_price,
        static_cast<int>(m_c6),
        static_cast<int>(StringToNumber(m_uidStr)),
        static_cast<int>(m_c7));
}