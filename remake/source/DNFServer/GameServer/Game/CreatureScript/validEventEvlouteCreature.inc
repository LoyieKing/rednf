bool CCreatureScript::validEventEvlouteCreature(int creatureId)
{
    // [b28d-solo] P96/P28：ORIG 0x0833f150 用**非 const** iterator
    // （ne 符号模板参 `IPi`=int*、begin 非 K 版），且增量段调 `ppEi`（后置）
    // ⇒ 源形 = `iterator` + `it++`。const_iterator 会多一条转换构造 call。
    for (std::vector<int>::iterator it = m_vec68.begin();
         it != m_vec68.end(); it++)
    {
        if (*it == creatureId)
        {
            return true;
        }
    }
    return false;
