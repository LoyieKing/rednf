bool CUserPremium::SetPremiumItemData(const unsigned long key, const STPremiumItemData& data)
{
    if (key == 0)
        return 1;
    // ORIG 0x86af70a：end() 先求值、find 后；未命中走 make_pair(lvalue,lvalue)
    // 引用对 + converting pair + insert（P123：非 operator[] 形）
    std::map<unsigned long, STPremiumItemData>::iterator e =
        g_map_premium_item_data.end();
    if (g_map_premium_item_data.find(key) == e)
    {
        g_map_premium_item_data.insert(std::make_pair(key, data));
        return 1;
    }
    return 0;
