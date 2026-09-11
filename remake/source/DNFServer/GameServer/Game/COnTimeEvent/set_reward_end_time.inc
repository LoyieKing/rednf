// ORIG @0x081bb994：*(time_t*)(this+0x54) = time(NULL) + param。
// ORIG 先把 time() 结果落 -0xc 栈槽再回读（time_t t = time(0); 两步）。
void COnTimeEvent::set_reward_end_time(int span)
{
    time_t t = time(0);
    m_rewardEndTime = t + span;
}