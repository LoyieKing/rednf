char const* CBoosterTest::getRarityStr(ENUM_RARITY rarity)
{
    switch (rarity)
    {
    case RARITY_COMMON:    return "common";
    case RARITY_UNCOMMON:  return "uncommon";
    case RARITY_RARE:      return "rare";
    case RARITY_UNIQUE:    return "unique";
    case RARITY_EPIC:      return "epic";
    case RARITY_CHRONICLE: return "chronicle";
    default:               return "unknown";
    }
