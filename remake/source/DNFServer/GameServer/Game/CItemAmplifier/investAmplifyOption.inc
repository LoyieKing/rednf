// ORIG 0x08234b30：在 m_script.m_options 中匹配 option.m_amount 类型的选项
// （stAmplifyOptionInfo_t::m_type == option.m_amount；option.m_value 不参与，
// 命中项自身的 +0x06 m_value 才是基数），命中则按 rarity 缩放并写
// inven.m_amp._setAbility(type, value, identified=true)。
void CItemAmplifier::investAmplifyOption(
    const CItem* item,
    Inven_Item& inven,
    const stInvestAmplifyOption_t& option) const
{
    if (option.m_amount == 0)
    {
        return;
    }

    std::vector<stAmplifyOptionInfo_t>::const_iterator it =
        m_script.m_options.begin();
    std::vector<stAmplifyOptionInfo_t>::const_iterator end =
        m_script.m_options.end();
    for (; it != end; ++it)
    {
        // [b28c-16] P222 正向包裹：ORIG `cmp; sete; je <++it 岛>`（命中 fall-through），
        // 写 `!= continue` 4.4 反产 setne/je 命中块；type 不设局部（ORIG 直接
        // movzbl %al,%eax 内联进实参）。
        if (it->m_type == option.m_amount)
        {
            unsigned short value =
                (unsigned short)((float)it->m_value *
                                 m_script.m_rarityRate[item->get_rarity()]);
            inven.m_amp._setAbility((unsigned char)option.m_amount, value, true);
            return;
        }
    }
}