bool CEventCreateDnf::IsRewardCondition()
{
    // ORIG 0x08164092：call GetState() 判 ==2，再比较 16 位 m_rate==100。
    // P56：bool 返回分支形态（cmp/jcc/mov $1/jmp/mov $0 双层汇聚），
    // 直 return 表达式只生成单层汇聚 → 写 if/return。
    // 帧差：ORIG sub $0x18，本工具链 sub $0x4（gcc4.1/4.4 栈槽布局差异，
    // P112/跨版本豁免，语义与指令序列逐条一致）。
    if (GetState() == State::kReward && m_rate == 100)
        return true;
    return false;
