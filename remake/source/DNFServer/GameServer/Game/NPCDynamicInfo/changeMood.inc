// ORIG 0x085804b2 T：
//   flag 假时校验 m_fieldc <= getCurSec() 才更新；更新 = getNextMood 轮转 +
//   m_fieldc 累加当前秒。返回当前 mood（enum，直传 getMood eax）。
ENUM_NPC_MOOD CNPCDynamicInfo::changeMood(bool flag)
{
    if (flag || m_fieldc <= GlobalData::s_systemTime_.getCurSec()) {
        m_fieldc = m_script->getNextMood(m_moodState, m_field8);
        m_fieldc += GlobalData::s_systemTime_.getCurSec();
    }
    return getMood();
}