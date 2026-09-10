CParser::CParser()                              // 0x082353a4
    : m_cmdName(), m_args(), m_line(), m_extra()
{
    // ORIG 仅构造 3 个 string 与 +0x11c vector；m_user/m_pos/m_id/m_buf 不初始化
    //（由 GM 入口填充，ORIG 行为如此）。
