int getRandomChampionElement(const STMonsterScript* script, int seed)
{
    int result = 4;
    if (script == 0)
        return result;                         // 空指针守卫
    const int count =
        static_cast<int>(script->m_championElements.size());    // +0xc40
    srand(seed);                               // 全局种子污染与 ORIG 一致
    if (count == 0)
        result = rand() % 4 + 1;               // 有符号取模后 +1
    else
        result = script->m_championElements[rand() % count];
    return result;
