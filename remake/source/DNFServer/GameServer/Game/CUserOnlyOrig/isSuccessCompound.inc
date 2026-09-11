// ORIG 0x08671fde T _ZN5CUser17isSuccessCompoundEi（报告 docs/CUser/isSuccessCompound.md）
// [b28d-22 N/P197④] ORIG 0x08672031 `cmp 0xc(%ebp),%eax; jge <mov$0 尾块>`（4.1 对
// 寄存器比较结果直跳）。4.4 对 `chance <= get_rand_int(100)` 恒物化 setge+test+je
// （探针 /tmp/p22_ge.cpp a..g 六形：setge/setl + 双 mov 岛，无直跳形）⇒ 封顶；
// 语义等价（chance<=rand ⇔ rand>=chance）。
// [b28d-solo-CUserOnlyOrig N 封顶] ORIG 0x08671fde：N（P197④ 镜像负例）——ORIG `cmp 0xc(%ebp),
// %eax; jge <mov$0 尾块>` 直跳；4.4 对 `chance <= rand` 与正极性 `rand >= chance` **同产**
// `setge+test`（本波 fresh 复验，34 条；反向 `chance > rand` 产 setl）⇒ 源形不可择，语义等价。
bool CUser::isSuccessCompound(int chance)
{
    if (getRecipeProbability() == 1)
        return true;
    if (getRecipeProbability() == 2)
        return false;
    // [b28d-solo-CUserOnlyOrig P258/P214①] ORIG 0x0867201c `cmpl $0,chance; je <mov$1>; …
    // call; cmp chance,%eax; jge <mov$0>` = `||` 短路直跳形；嵌套 if 形在 4.4 恒物化
    // setge+test（旧 34 条）。改 `(chance==0) || (rand < chance)` 复现直跳（探针 p_rand f5）。
    return (chance == 0) || (get_rand_int(100) < chance);
}