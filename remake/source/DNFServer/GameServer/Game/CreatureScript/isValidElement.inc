// ORIG 0x88907fe：对集合中每个元素（除 4）在 STComputeDamageVariable::m_elemEnable
// 置 is-valid 标记，调用 isValidAttackInfoElement(STComputeDamageVariable*, int)。
bool AttackInfo::isValidElement(ENUM_ELEMENT element)
{
    STComputeDamageVariable v;
    for (std::set<ENUM_ELEMENT>::const_iterator it = m_elements.begin();
         it != m_elements.end(); it++)
    {
        ENUM_ELEMENT e = *it;
        // [b28c-L7] P222 正向包裹：ORIG 0x08890846 `cmpl $4; je <inc块>` 且
        // body 尾**无 jmp**（自然 fall-through 到 inc）。`==NONE → continue`
        // 形在 4.4 的 inc 块落点不同 ⇒ body 尾多 `jmp + nop` 中继对。
        if (e != ENUM_ELEMENT_NONE)
            v.m_elemEnable[e] = 1;
    }
    return isValidAttackInfoElement(&v, element);
}