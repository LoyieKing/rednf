// _Z31getCreatureDamageRateFromModule16ENUM_MODULE_TYPE @0x08095430 (0x6c)：
// map find 命中返 value，未命中 1.0f。
float getCreatureDamageRateFromModule(ENUM_MODULE_TYPE type)
{
    std::map<ENUM_MODULE_TYPE, float>::iterator it =
        g_creatureModuleDamageRateMap.find(type);
    if (it != g_creatureModuleDamageRateMap.end())
        return it->second;
    return 1.0f;
}