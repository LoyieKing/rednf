float getCreatureDamageRateFromModule(ENUM_MODULE_TYPE type)
{
    std::map<ENUM_MODULE_TYPE, float>::iterator it =
        g_creatureModuleDamageRateMap.find(type);
    if (it != g_creatureModuleDamageRateMap.end())
        return it->second;
    return 1.0f;
