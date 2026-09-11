// STExpertJobScript::GetRecipeInfo（ORIG 0x0832b6b6 W）。
// 在 m_recipeInfoMap（+0x200，map<int,stRecipeInfo>）按 index 查；
// 找到返回 &it->second，否则 0。
stRecipeInfo* STExpertJobScript::GetRecipeInfo(int index)
{
    std::map<int, stRecipeInfo>::iterator it = m_recipeInfoMap.find(index);
    // ORIG 0x832b6f9 ne + 命中 fall-through；je→0x832b71f mov$0 尾岛
    if (it != m_recipeInfoMap.end())
        return &it->second;
    return 0;
}