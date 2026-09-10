bool CStackableItem::verifyStackableAction(StackableAction::Type type) const
{
    // b28c-L8 判词 [N(N18a)]：ORIG 0x0822ca58 = `mov 0x39c(%eax),%eax; mov %eax,%edx;
    // mov 0xc(%ebp),%eax; cmp %eax,%edx`（字段读两段），4.4 恒折单段直落 %edx；
    // (int)/(unsigned) cast 互换与 std::underlying_type 形均不改（N18a 反向判词：
    // `mov mem,%eax; mov %eax,%reg` 单段 regmove 不可造）⇒ 残 1 行封顶。
    // [b28d-P254 翻案] m_int39c 改 StackableAction::Type（enum，CStackableItem.h）
    // 后 4.4 复现两段装载 ⇒ 本件 IDENTICAL（10=10）。旧 N18a 判词作废。
    return m_int39c == type;
