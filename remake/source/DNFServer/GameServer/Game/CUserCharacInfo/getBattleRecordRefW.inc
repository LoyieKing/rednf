std::map<int, GameResultType>* CUserCharacInfo::getBattleRecordRefW()
{
    enableSaveBattleRecord();
    return &m_battleRecord;
}