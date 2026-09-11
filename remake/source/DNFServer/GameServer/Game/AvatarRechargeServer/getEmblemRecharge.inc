int ARAD::SCRIPT::AvatarRecharge::getEmblemRecharge(int grade)
{
    // ORIG 0x088b132a：+0x1c map 值或 0。
    // b28d-lane25：P167 差集实证 ORIG 用非 const iterator + 先声明后赋值（C1Ev）
    // + `!=` 正极性（P50 反例复活 / P96）。
    std::map<int, int>::iterator it;
    it = m_emblemRechargeMap.find(grade);
    if (it != m_emblemRechargeMap.end())
        return it->second;
    return 0;
}