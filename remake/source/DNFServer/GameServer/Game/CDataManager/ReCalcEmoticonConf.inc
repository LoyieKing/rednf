void CDataManager::ReCalcEmoticonConf(CChattingEmoticonConfig& config, const CUser& user)
{
    m_emoticonList->ReCalc(config, user);
