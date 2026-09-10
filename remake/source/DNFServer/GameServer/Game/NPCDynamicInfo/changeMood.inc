ENUM_NPC_MOOD CNPCDynamicInfo::changeMood(bool flag)
{
    if (flag || m_fieldc <= GlobalData::s_systemTime_.getCurSec()) {
        m_fieldc = m_script->getNextMood(m_moodState, m_field8);
        m_fieldc += GlobalData::s_systemTime_.getCurSec();
    }
    return getMood();
