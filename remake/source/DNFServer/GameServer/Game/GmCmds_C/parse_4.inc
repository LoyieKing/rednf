// ORIG parse 0x084ac720：atoi(args) → +0x08；+0x0c、+0x10 显式清零。
void CGuildDebug::parse()
{
    trim(m_parser->m_args);
    m_param1 = atoi(m_parser->m_args.c_str());
    m_param2 = 0;
    m_f1 = 0;  // +0x10（ORIG 无其它写点，恒为 0）
}