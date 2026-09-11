// ORIG T 0x0812f770：state==3 || state==4。
bool ProcStage::isRewardState() const
{
    return m_state == 3 || m_state == 4;
}