// [b28d-solo-CUserPremium] IDENTICAL/AE（routed 口径 check_tu_routed 复验）：
// ORIG 0x086af70a 逐条对齐（52/52，ndiff=0）。P167 唯一差 = make_pair 模板
// 实参 IRKm vs IRm ⇒ 定义处形参加 top-level const（P315：const 左值实参令
// make_pair 推导 IRKm；top-level const 不入函数类型/mangled，声明无需同步）。
// 终判（b28d-solo-CUserPremium）：IDENTICAL 52/52、P167 CLEAR（6/6）。
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
}