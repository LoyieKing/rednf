bool isValidAttackInfoElement(STComputeDamageVariable* v, int elem);

// ORIG 0x88907fe：对集合中每个元素（除 4）在 STComputeDamageVariable::m_elemEnable
// 置 is-valid 标记，调用 isValidAttackInfoElement(STComputeDamageVariable*, int)。
