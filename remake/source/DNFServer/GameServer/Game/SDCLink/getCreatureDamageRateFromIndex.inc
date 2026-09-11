// _Z30getCreatureDamageRateFromIndexi @0x0809549c (0x6c)：同型 map<int,float>。
float getCreatureDamageRateFromIndex(int index)
{
    std::map<int, float>::iterator it = g_creatureIndexDamageRateMap.find(index);
    if (it != g_creatureIndexDamageRateMap.end())
        return it->second;
    return 1.0f;
}