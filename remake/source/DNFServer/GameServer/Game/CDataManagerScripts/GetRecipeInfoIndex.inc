// STExpertJobScript::GetRecipeInfoIndex（ORIG 0x0832b726 W）。
// 在 m_recipeIndexMap（+0x218，map<unsigned long,int>）按 itemIndex 查；
// 找到返回 mapped int（recipe 信息索引），否则 -1。
int STExpertJobScript::GetRecipeInfoIndex(unsigned long itemIndex)
{
    std::map<unsigned long, int>::iterator it = m_recipeIndexMap.find(itemIndex);
    // ORIG 0x832b765 ne + 命中 fall-through；未命中 je→0x832b78f mov $-1 尾岛
    if (it != m_recipeIndexMap.end())
        return it->second;
    return -1;
}