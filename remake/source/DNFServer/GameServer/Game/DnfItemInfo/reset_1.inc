// ORIG 0x822abea W：委托两 STEnchantedSkillOption::reset + m_field2 清零 +
// RandomOption::reset。定义置于本 TU（callee reset 均定义于 Inven_Item.cpp）：
// caller TU 不含 callee 定义体 ⇒ 4.4 unit-at-a-time 不塌缩 outgoing 帧，
// 保持 ORIG 的 sub $0x18（NL4 实证）。
void ChronicleRandomOptionInfo::reset()
{
    m_skill0.reset();
    m_skill1.reset();
    m_field2 = 0;
    m_random.reset();
}