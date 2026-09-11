// ============ Condition_LevelUp C1/D1【报告缺失·objdump 转写】 ============
// C1 0x0898605a：仅 _ZNSt6vectorIN17Condition_LevelUp11Reward_ItemsaIS1_EEC1Ev(this+4)；
//   忠实保留：m_filter0/m_filter1（+0x00/+0x01）不初始化（解析器写入；+1 字节为 growType 过滤）。
// D1 0x08986070：仅析构该 vector。
Condition_LevelUp::Condition_LevelUp()
    : m_items()
{
}