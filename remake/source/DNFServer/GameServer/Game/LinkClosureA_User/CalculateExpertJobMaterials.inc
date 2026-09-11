// ---------------------------------------------------------------------------
// CUser::CalculateExpertJobMaterials（报告全文已读）
// copy 向量上按 GetExpertJobCompoundMaterialVariation/100 缩减后再并回原表。
// P48 双迭代器形态照录（ci 在 copy 上、mi 在实参向量上同步推进）。
// ---------------------------------------------------------------------------
// b28d-65：残余 P0（jobLevel/jobType ORIG 直落 %ebx/%esi 无槽）+ N86（迭代器两段）+
//   槽向族；四形实测不可翻 ⇒ 封顶。
// [2026-09-11 本轮] 复测 fresh：orig=164 / our=164（gap 0）、ndiff **2** —— 仅
//   `lea -0x10(%ebp)` ↔ `lea -0x14(%ebp)` 两行互换（ORIG: ci++→-0x10 / mi++→-0x14）。
//   二度试交换 ci/mi 声明序 ⇒ ndiff 2→**14**（劣化，槽链 8 处错位）⇒ 回滚。
//   后置 ++ 两处 sret 临时槽向属 4.4 分配产物（N86），维持声明序。
void CUser::CalculateExpertJobMaterials(
    std::vector<std::pair<int, int> >& materials)
{
    // b28d-66 [P0]：ORIG jobLevel/jobType 直落 %ebx/%esi 且无栈槽 ⇒ 原码为内联右值，
    // 实参求值序（右到左）= level(含 GetCurCharacExpertJobExp) → type → 对象。
    float variation =
        getCurCharacInvenR()->GetExpertJobCompoundMaterialVariation(
            static_cast<ENUM_EXPERT_JOB_TYPE>(GetCurCharacExpertJobType()),
            GetCurExpertJobLevel(GetCurCharacExpertJobExp()));
                                                            // [fstps -0xc]
    std::vector<std::pair<int, int> > reduced(materials);   // [-0x28 vector copy]
    variation /= 100.0f;                                    // [fdiv _DAT_08cf3954]

    // b28d-66 [N86]：ORIG 迭代器 = 先声明（C1Ev ×2）后赋值（begin 经 -0x3c 临时中转
    // 再 mov 拷贝到 -0x2c/-0x30），非 4.4 的拷贝初始化直构。
    // [G5 2026-09-10 判据⑩试验·无效] 交换 ci/mi 声明序（期望翻转后置 ++ 的 sret 临时
    // -0x10/-0x14 相对序）实测四数不变（31/40/2/5）⇒ 已回滚，维持原声明序。
    // [2026-09-11 二次试形·仍无效] 交换 ci/mi 声明序实测 ndiff 2→14（劣化）⇒ 回滚。
    std::vector<std::pair<int, int> >::iterator ci;         // [-0x2c]
    std::vector<std::pair<int, int> >::iterator mi;         // [-0x30]
    ci = reduced.begin();
    mi = materials.begin();

    // b28d-66：ORIG 循环 = while(ci!=end && mi!=end)（无 xor $1 短路取反）+
    // 后置 ci++/mi++（_ZN…ppEi，sret 临时 -0x10/-0x14），非 do-while+break+前置 ++。
    while (ci != reduced.end() && mi != materials.end())   // [双 != 判定]
    {
        ci->second = static_cast<int>(                     // [fld/fistpl ROUND]
            static_cast<float>(ci->second) * variation);
        mi->second += ci->second;                          // [lea (%esi,%eax)]
        if (mi->second == 0)                               // [test/sete]
        {
            mi->second = 1;
        }
        ci++;                                              // [operator++(int) ×2]
        mi++;
    }
}