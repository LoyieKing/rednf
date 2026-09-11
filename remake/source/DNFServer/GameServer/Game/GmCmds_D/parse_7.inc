// ORIG parse 0x084b3db2：parser 非空时 extra[1]→字符串，extra[2]/extra[3] 经
// StringToNumber 写 +0x0c/+0x10。
void CHeroMissionCommand::parse()
{
    if (m_parser != 0)
    {
        m_str0 = m_parser->m_extra[1];
        m_missionNo = static_cast<unsigned int>(
            StringToNumber(m_parser->m_extra[2].c_str()));
        m_param = static_cast<unsigned int>(
            StringToNumber(m_parser->m_extra[3].c_str()));
        return;                                             // ORIG 尾 `jmp;leave;ret`（4.1 显式 return 形）
    }
    return;
}