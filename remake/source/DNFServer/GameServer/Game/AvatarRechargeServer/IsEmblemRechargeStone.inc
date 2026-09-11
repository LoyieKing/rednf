bool ARAD::SCRIPT::AvatarRecharge::IsEmblemRechargeStone(int itemIndex)
{
    // ORIG 0x088b13a6：+0x04 map find 命中返回 true。
    // P56：ORIG 尾部 choose 岛（test;je;mov$1;jmp;mov$0）⇒ if-return 形。
    if (m_emblemRechargeStoneMap.find(itemIndex) != m_emblemRechargeStoneMap.end())
        return true;
    return false;
}