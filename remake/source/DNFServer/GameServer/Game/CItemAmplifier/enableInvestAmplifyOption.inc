// ORIG 0x08234c3c：返回 ENUM_ERROR 码（0 = 允许）。
//   0:  item 无能力且未强化 → 0；有强化 → 0x12；有(已识别)能力 → 0x14
//   1:  有能力 → (有强化 → 0x12；否则 0)；无能力 → 0x15
//   2:  恒 0
//   其他（>2）: 恒 1
int CItemAmplifier::enableInvestAmplifyOption(
    const Inven_Item& inven,
    ENUM_ITEM_AMPLIFY_INVEST_TYPE type) const
{
    switch (type)
    {
    case ENUM_ITEM_AMPLIFY_INVEST_TYPE_2:
        break;
    case ENUM_ITEM_AMPLIFY_INVEST_NONE:
        if (inven.m_amp.hasAbility())
        {
            return 0x14;
        }
        if (inven.GetUpgrade())
        {
            return 0x12;
        }
        break;
    case ENUM_ITEM_AMPLIFY_INVEST_TYPE_1:
        if (!inven.m_amp.hasAbility())
        {
            return 0x15;
        }
        if (inven.GetUpgrade())
        {
            return 0x12;
        }
        break;
    default:
        return 1;
    }
    return 0;
}