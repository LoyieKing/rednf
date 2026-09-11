// Logout：ORIG 0x86840c4。"IP-,%s,%d,%d,%d,%d,%d,%p"，
// 参数顺序：characName, a3, a4, (int)a6(uchar), server_group(CEnvironment), a5, this指针。
void cUserHistoryLog::Logout(const char* a, const char* b, int c, int d, int e,
                             unsigned char f)
{
    // ORIG 0x08685bxx：get_server_group 结果留 callee-saved（%ebx）直转实参；
    // 4.4 命名局部必 spill 槽（frame +0x14）。取链式内联形。
    m_user->LogHistory("IP-,%s,%d,%d,%d,%d,%d,%p",
                       b, c, d, static_cast<int>(f),
                       G_CEnvironment()->get_server_group(), e,
                       (const void*)m_user);
}