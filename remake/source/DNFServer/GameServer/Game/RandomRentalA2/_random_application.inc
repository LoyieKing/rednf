// m_map0 查 (rarity,level)：命中时 roll=randInt(m_field4+m_field0)，
// m_field4 < roll → 1（应用），否则 0；未命中 0。
// （ORIG 反汇编实证：bound=pt()+0x8 + pt()+0x4（value.m_field4+m_field0），
//   阈值=pt()+0x8（m_field4）；2026-08-30 修正字段选择。）
// [Lane13] 返回类型 int→bool（CItemGenerator.h 同步改）：ORIG choose_item 0x085f163d
// 调用点 `call; xor $0x1,%eax; test %al,%al; je` = bool 直测（P292/P248档①）。
bool CRandomOptionItemHandle::_random_application(ENUM_RARITY rarity, int level)
{
    // [b28d-solo-RandomRentalA2 判词] AE 67/67（ORIG 0x085f0ec2）：P0 bound 内联
    // 匿名临时 + 删 char framePad；key/it/end/bound/roll 槽与指令逐条全等。
    RandomApplicationKey key((unsigned char)rarity, (unsigned char)level);
    // [b28d-solo-RandomRentalA2] P0/P48：ORIG 两次独立调用 GetRandomOption
    // （find / end 各一次，`mov %eax,%edx` 寄存器直传无槽），去 table 局部。
    std::map<RandomApplicationKey, RandomApplicationValue>::const_iterator it =
        reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_map0.find(key);
    if (it != reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_map0.end())
    {
        // [b28d-solo-RandomRentalA2 P0] ORIG bound@-0x10 = randInt 匿名临时
        // （`mov %eax,-0x10; lea -0x10(%ebp),%edx`）；具名局部恒落 -0x20 并挤走
        // end 临时槽 ⇒ 内联实参。
        const unsigned long roll = static_cast<unsigned long>(
            m_rand->randInt(static_cast<unsigned long>(
                it->second.m_field4 + it->second.m_field0)));
        if (static_cast<unsigned int>(it->second.m_field4) < roll)
        {
            return 1;
        }
    }
    return 0;
}