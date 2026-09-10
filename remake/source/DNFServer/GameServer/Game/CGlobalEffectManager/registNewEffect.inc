void CGlobalEffectManager::registNewEffect(unsigned int type, float value,
                                           const char* name, long beginSec,
                                           long reserveSec)
{
    if (isAffectedEffect(type))
        return;

    if (beginSec == 0)
    {
        long endTime = GlobalData::s_systemTime_.getCurSec() + reserveSec;
        CGlobalEffect effect(type, value, name, endTime);
        m_activeEffects.push_back(std::move(effect));   // ORIG push_back(EOS0_) 移动
        notifyGlobalEffectInfo();
        TimerGlobalEffectItemTimeout::registTimer(type, reserveSec);
    }
    else
    {
        long endTime = GlobalData::s_systemTime_.getCurSec() + beginSec + reserveSec;
        CGlobalEffect effect(type, value, name, endTime);
        m_reservedEffects.push_back(std::move(effect));   // ORIG push_back(EOS0_) 移动
        TimerGlobalEffectItemBegin::registTimer(type, beginSec);
        TimerGlobalEffectItemTimeout::registTimer(type, beginSec + reserveSec);
    }
