stRecipeInfo* STExpertJobScript::GetRecipeInfo(int index)
{
    std::map<int, stRecipeInfo>::iterator it = m_recipeInfoMap.find(index);
    // ORIG 0x832b6f9 ne + 命中 fall-through；je→0x832b71f mov$0 尾岛
    if (it != m_recipeInfoMap.end())
        return &it->second;
    return 0;
