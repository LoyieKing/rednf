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
