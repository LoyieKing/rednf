void CPowerWarPacketLog::PrintPowerWarPacketLogReport()
{
    // b28d-60：P221 全表达式临时链——ORIG 槽深序 trace1 -0x7c → trace3 -0x5c
    //（先声明更深），具名声明在 4.4 恒反向（-0x2c → -0x4c）⇒ 改「构造即调用」。
    cMyTrace(__PRETTY_FUNCTION__, 0x3a1, 0)("-------------------------------------------------------------------------------------------------------------");
    cMyTrace(__PRETTY_FUNCTION__, 0x3a2, 0)("|                                   POWAR WAR PACKET RESULT REPORT                                          |");
    cMyTrace(__PRETTY_FUNCTION__, 0x3a3, 0)("-------------------------------------------------------------------------------------------------------------");

    std::vector<STPacketLog*> vec;
    for (std::map<int, STPacketLog*>::iterator it = m_map.begin();
         it != m_map.end(); ++it)
    {
        vec.push_back(it->second);
    }
    std::sort(vec.begin(), vec.end(), STPacketLog::Compare);

    for (std::vector<STPacketLog*>::iterator it = vec.begin();
         it != vec.end(); ++it)
    {
        STPacketLog* log = *it;
        cMyTrace(__PRETTY_FUNCTION__, 0x3b1, 0)(
            "- PACKET CLASS:%d NO:%5d, COUNT:%8d, NAME:%s",
            log->m_field4, log->m_field8, log->m_count, log->m_name.c_str());
    }

    cMyTrace(__PRETTY_FUNCTION__, 0x3b6, 0)("-------------------------------------------------------------------------------------------------------------");
