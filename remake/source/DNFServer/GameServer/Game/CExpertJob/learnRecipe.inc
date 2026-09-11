// learnRecipe 0x0832b3c0：GetRecipeInfoIndex(recipeIndex)；index<0 → 0；
// GetRecipeInfo(index) 为空 → 0；recipeInfo->m_field8 <= param+2 才 set_recipe + alter。
// b28c-1 N: 符号位测试 4.4 恒折 cmpl/jns vs ORIG mov/shr/test（跨版本 lowering）。
// [b28d-62] AE：P266 字节收窄符号测试（P198① 预清）后 46=46，p167 CLEAR。
//   ORIG 0x0832b3c0
bool CCharacExpertJob::learnRecipe(unsigned long recipeIndex, STExpertJobScript* script,
                                  int param)
{
    int idx = -1;                                   // ORIG：-1 初始化
    idx = script->GetRecipeInfoIndex(recipeIndex);
    // b28：ORIG 判负形为 `(unsigned)idx >> 31` 提取符号位 + test/je 岛（P222 互试）。
    // [b28d-62] 加 (unsigned char) 收窄：4.4 对 int 版恒折 `cmpl $0,-0x10; jns`；
    //   ORIG 是 `mov idx,%eax; shr $0x1f,%eax; test %al,%al; je`（P266 字节比较形）。
    if ((unsigned char)((unsigned int)idx >> 31) != 0)
        return 0;
    stRecipeInfo* info = script->GetRecipeInfo(idx);
    if (info == 0)
        return 0;
    // b28 P222 互试：正极性 `mf8 > param+2 → 0`（ORIG cmp %edx,%eax; jle continue）。
    if ((int)(unsigned int)info->m_field8 > param + 2)
        return 0;
    set_recipe(idx);
    alter();
    return 1;
}