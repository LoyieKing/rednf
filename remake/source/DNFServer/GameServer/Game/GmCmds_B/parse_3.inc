// ORIG parse 0x084aadd8：trim(args)；pos=args.find(" ")；
// *(int*)(this+8)=atoi(substr(0,pos))；*(int*)(this+0xc)=atoi(substr(pos+1))。
void CChangeNPCFavorCmd::parse()
{
    trim(m_parser->m_args);
    int pos = m_parser->m_args.find(" ", 0);
    std::string first = m_parser->m_args.substr(0, pos);
    std::string second = m_parser->m_args.substr(pos + 1, std::string::npos);
    // b28d-22：P0 — ORIG 0x084aadd8 把 atoi 结果直接写成员（无 v0/v1 槽，
    // 无 store+load 往返），删两个具名中转局部。
    m_npcId = atoi(first.c_str());
    m_favor = atoi(second.c_str());
}