// ============================================================================
// CItemMakingSkill::GetPrevExpGap —— ORIG T 0x08545b7e（OrigItemGen20 批）。
// 报告 docs/class_func_reports/CItemMakingSkill/GetPrevExpGap.md：
//   G_CDataManager()->GetItemMakingSkill() 得技能经验表宿主（stItemMakingSkill，
//   +0x18 为 vector<pair<int,int>> 档位表）；idx = level - 1；
//   守卫 (unsigned)idx >= size() || idx == 0 → 返回 0（ORIG cmp/jbe + cmpl/jne
//   双判，jbe 为无符号比较）；否则返回 档位[idx].first - 档位[idx-1].first
//   （两次 vector::operator[]，idx 与 idx-1；ORIG mov (%eax),%ebx/sub 形态）。
// ============================================================================
// [b28d-solo-CItemMakingSkill] 修复记录（旧 [b28d-D43] 的 N82/N95「槽平移/idx 寄存器
//   直比不可复现」判词作废）：
//   ① 守卫按 ORIG 方向写 `size() <= (unsigned)idx || idx == 0`——P209：unsigned 比较
//      在 4.4 恒先载寄存器；写成 `(unsigned)idx >= size()` 多 `mov idx,%ebx` 且比较方向反。
//   ② idx 局部声明为 unsigned int（P218 扩展）：同时翻正 4.4 的
//      `cmp -0xc(%ebp),%eax`（内存直比、无中转）与主流程 receiver 的 ORIG 形
//      `mov table,%eax; lea 0x18(%eax),%edx; mov idx,%eax`。
//   ③ table 声明为 `const stItemMakingSkill*`（P107/P270 符号级 cv 判据）：
//      p167 差集实证 ORIG 调 `_ZNKSt6vector…ixEj`（const operator[]），
//      非 const 指针会发 `_ZNSt6vector…ixEj`（2 处符号级错配）。
//   语义：level<=0 ⇒ idx=0xFFFFFFFF ⇒ 命中守卫返回 0，与 ORIG 无符号比较逐位一致；
//   主流程 idx/idx-1 作 vector 下标同值。
int CItemMakingSkill::GetPrevExpGap(int level)
{
    const stItemMakingSkill* table =
        reinterpret_cast<const stItemMakingSkill*>(G_CDataManager()->GetItemMakingSkill());
    unsigned int idx = static_cast<unsigned int>(level - 1);
    if (table->m_vec18.size() <= idx || idx == 0)
        return 0;
    return table->m_vec18[idx].first - table->m_vec18[idx - 1].first;
}