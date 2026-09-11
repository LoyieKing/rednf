// ---- _get_base_item —— T 0x085f1426（同名报告全文转写）-------------------
int CRandomOptionItemHandle::_get_base_item(ENUM_RARITY rarity, int limit)
{
    // [b28d-solo-RandomRentalA2 判词] AE 92/92（ORIG 0x085f1426）：P0 bound 内联
    // 匿名临时 + 删 char framePad + P222 正向包裹 + 迭代器循环，逐条全等。
    RandomApplicationKey key((unsigned char)rarity, (unsigned char)limit);
    // [b28d-solo-RandomRentalA2] P0/P48：ORIG 每处 map 访问独立调 GetRandomOption。
    std::map<RandomApplicationKey, BaseItemSelectionValue>::const_iterator it =
        reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_map48.find(key);
    // P222：ORIG `je <fail 岛>` + body 直落 ⇒ 正向包裹。
    if (it != reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_map48.end())
    {
        // [b28d-solo-RandomRentalA2 P0] ORIG bound = randInt 匿名临时（同族）。
        unsigned int r = (unsigned int)m_rand[3].randInt(
            (unsigned long)it->second.m_fieldC);

        // ORIG 为迭代器循环（begin 槽 -0x28 / 每轮重调 end 槽 -0x14 /
        // operator* 取元素 / 独立 idx 槽 -0xc），非 size() 索引循环。
        unsigned int idx = 0;
        for (std::vector<unsigned int>::const_iterator i = it->second.m_vec0.begin();
             i != it->second.m_vec0.end(); ++i, ++idx)
        {
            if (r < *i)
            {
                return (int)idx;
            }
        }
    }
    return 0;
}