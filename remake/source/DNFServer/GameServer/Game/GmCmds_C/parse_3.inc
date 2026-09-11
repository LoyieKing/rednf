// ORIG parse 0x084b28d6：args=="gw" 时置重置标记，否则标记清零并 atoi 存时计值。
void CGrowhWeaponEvent::parse()
{
    trim(m_parser->m_args);
    if (m_parser->m_args == "gw")
    {
        m_resetFlag = 1;
    }
    else
    {
        m_resetFlag = 0;
        m_timepiece = atoi(m_parser->m_args.c_str());
    }
}