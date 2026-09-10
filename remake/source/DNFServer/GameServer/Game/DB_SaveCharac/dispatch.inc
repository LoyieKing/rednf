bool DB_SaveCharac::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_SAVE_CHARAC* sig = stream->GetOutBuffer<SIG_SAVE_CHARAC>();
    if (sig == 0)
        return false;
    if (sig->m_bSaveInfo != 0 || sig->m_bSaveInfoSub != 0)
    {
        if (_SaveCharacInfo(sig) != 1)
            return false;
    }
    if (sig->m_bSaveManageInfo != 0)
    {
        if (_SaveCharacManageInfo(sig) != 1)
        {
            LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x3f7d,
                                  "[SAVE CHARAC MANAGE INFO ERROR] characNo(%u), "
                                  "tagCharacNo(%u), skillIndex(%u)",
                                  sig->m_characNo, sig->m_tagCharacNo,
                                  (unsigned int)sig->m_strikerSkillIndex);
        }
    }
    if (sig->m_bSaveStat != 0)
    {
        if (_SaveCharacStat(sig) != 1)
            return false;
    }
    if (sig->m_bSaveCubePremiumInfo != 0)
        _SaveContractOfCubePremiumInfo(*sig);
    return true;
