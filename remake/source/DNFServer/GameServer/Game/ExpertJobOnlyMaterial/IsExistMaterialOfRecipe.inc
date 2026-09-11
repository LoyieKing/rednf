// ORIG _ZN10expert_job17ExpertJobMaterial23IsExistMaterialOfRecipeEv
// @0x0849c166：配方成立且背包满足整份材料需求才通过。
// b28d-solo-ExpertJobOnlyMaterial: AE — P225 while+break 复现槽比零
// `cmpl;setne;test;je` 物化；P292 档① CheckNeedItemList bool 化消 test/sete。
int ExpertJobMaterial::IsExistMaterialOfRecipe()
{
    if (Empty())
        return 0x11;

    int recipeErr = 0;
    recipeErr = IsRecipe();   // ORIG 0x849c17b 前置 movl $0x0,-0xc 预清（P198-①）
    // b28d-36（P225 试形）：ORIG 0x849c183 `cmpl $0x0,-0xc; setne %al;
    // test %al,%al; je` 为 4.1 槽比零 bool 物化；4.4 对 `if (x != 0)` 恒折
    // 直跳。while+break 形复现该四段物化（P225 定稿）。
    while (recipeErr != 0)
    {
        return recipeErr;
    }

    if (reinterpret_cast<CUser*>(m_user)->getCurCharacR() == 0)
        return 3;

    // ORIG 0x849c1cb：materials 经 lea 0x174(%eax) 内联（P49：勿经 static inline helper）；
    // 0x849c1ea `call; xor $1,%eax; test %al,%al; je` ⇒ callee 返回 bool
    // （P292 档①；CInventory.h/.cpp 的 CheckNeedItemList 已由 Main 代落 bool）。
    if (!reinterpret_cast<CUser*>(m_user)->getCurCharacInvenR()->CheckNeedItemList(
            reinterpret_cast<CStackableItem*>(m_item)->m_recipe174.m_vec0))
        return 0x15;
    return 0;
}