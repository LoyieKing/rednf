// ORIG _ZN15cUserHistoryLog16AntibotPacketLogEN20AntibotPacketLogType1TE @0x08686dd0
void cUserHistoryLog::AntibotPacketLog(AntibotPacketLogType::T type)
{
    m_user->LogHistory("AntibotPacketLog,%d", (int)type);
}