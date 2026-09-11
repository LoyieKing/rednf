// ORIG isResetState 0x08164fce：movzbl *(this+0x11) = condition 字节 +0x09。
char BaseHeroMissionCondition::isResetState()
{
    return m_condition.m_isResetState;
}