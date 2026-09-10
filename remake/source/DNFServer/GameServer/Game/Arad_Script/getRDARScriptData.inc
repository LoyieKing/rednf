std::vector<_AradJumpingRewardItem>* AradJumping_Script::getRDARScriptData()
{
    return &m_rewardItems;   // 【objdump】W 0x088b43ee：mov %eax,%eax … 返回 this+0。
