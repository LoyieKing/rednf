int CQuestShop::getInitCost(int count)
{
    // ORIG 0x85ef1ba：G_CDataManager() 每次重读（三次独立 call）
    const int cost0 = G_CDataManager()->m_questShop.m_initCost.m_cost0; // dm+0x63b4
    const int cost1 = G_CDataManager()->m_questShop.m_initCost.m_cost1; // dm+0x63b8
    const int cost2 = G_CDataManager()->m_questShop.m_initCost.m_cost2; // dm+0x63bc
    if (count == 0)                               // +0x30 jne
        return cost0;                             // -0x18
    const int total = count * cost1 + cost0;      // imul/add -0xc
    if (total >= cost2)                           // ORIG jl 跳 return total（P47 布局）
        return cost2;                             // cap
    return total;
