void cUserHistoryLog::AntibotPacketLog(AntibotPacketLogType::T type)
{
    m_user->LogHistory("AntibotPacketLog,%d", (int)type);
