// ORIG parse 0x084a98c8：trim(args)；*(int*)(this+8) = atoi(args.c_str())。
void CChangeExpertJobCmd::parse()
{
    trim(m_parser->m_args);
    m_jobType = atoi(m_parser->m_args.c_str());   // P0：无具名局部（ORIG 无槽）
}