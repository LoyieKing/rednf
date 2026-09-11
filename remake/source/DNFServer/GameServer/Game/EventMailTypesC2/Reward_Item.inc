// ============ Condition_LevelUp::Reward_Item C1/D1【报告缺失·objdump 转写】 ======
// C1 0x08986086：_ZNSsC1Ev(this+0xc) → _ZNSsC1Ev(this+0x10)；忠实保留：
//   m_level/m_itemIndex/m_itemCount（+0x00..+0x0b 三个 int）不初始化。
// D1 0x089860d2：逆序析构 string(+0x10) → string(+0xc)。
Condition_LevelUp::Reward_Item::Reward_Item()
    : m_title(),
      m_content()
{
}