std::string ServiceRestrictManager::serviceRestrictStringQuery()
{
    std::ostringstream oss;
    for (int i = 0; i <= 0x21; ++i)
    {
        oss << "(" << 1 << "," << i << ",";
        oss << MobileRestrict::enumToString(static_cast<MobileRestrict::Enum>(i));
        oss << ")";
    }
    for (int i = 0; i <= 2; ++i)
    {
        oss << "(" << 2 << "," << i << ",";
        oss << CharacterRestrict::enumToString(static_cast<CharacterRestrict::Enum>(i));
        oss << ")";
    }
    return oss.str();
