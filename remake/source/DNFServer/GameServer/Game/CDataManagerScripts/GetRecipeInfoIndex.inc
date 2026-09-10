int STExpertJobScript::GetRecipeInfoIndex(unsigned long itemIndex)
{
    std::map<unsigned long, int>::iterator it = m_recipeIndexMap.find(itemIndex);
    // ORIG 0x832b765 ne + 命中 fall-through；未命中 je→0x832b78f mov $-1 尾岛
    if (it != m_recipeIndexMap.end())
        return it->second;
    return -1;
