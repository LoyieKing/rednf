// ORIG 0x85e6adc（W）：mov 0x40(%eax),%eax; ret —— 直返 m_clearCount。
int CMission_rank::getClearCondition() const
{
    return m_clearCount;
}