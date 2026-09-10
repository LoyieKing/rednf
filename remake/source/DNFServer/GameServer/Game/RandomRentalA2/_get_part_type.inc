int CRandomOptionItemHandle::_get_part_type(int limit)
{
    // [b28d-solo-RandomRentalA2 判词] AE 84/84（ORIG 0x085f1088）：P0 bound 内联
    // 匿名临时（`randInt((unsigned long)it->second.m_fieldC)`）+ P222 正向包裹 +
    // 迭代器循环；bound/end 槽归位。
    unsigned int r;
    std::map<int, PartSelectonValue>::const_iterator it =
        reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_map30.find(limit);
    // P222：ORIG `je <fail 岛>` + body 直落 ⇒ 正向包裹（早返形多 mov$0/jmp）。
    if (it != reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_map30.end())
    {
        // [b28d-solo-RandomRentalA2 P0] ORIG bound@-0x14 = randInt 匿名临时
        // （`mov %eax,-0x14; lea -0x14,%eax`），具名局部会挤走 end 临时槽。
        r = (unsigned int)m_rand[2].randInt((unsigned long)it->second.m_fieldC);

        // ORIG 为迭代器循环（begin 槽 -0x20 / 每轮重调 end 槽 -0x10 /
        // operator-> 取 m_weight、m_partType），非 size() 索引循环；
        // 且每次重取 it->second.m_vec0（无 vec 引用局部）。
        for (std::vector<PartSelectonField>::const_iterator i = it->second.m_vec0.begin();
             i != it->second.m_vec0.end(); ++i)
        {
            if (r < (unsigned int)i->m_weight)
            {
                return i->m_partType;
            }
        }
    }
    return 0;
