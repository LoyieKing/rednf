// ORIG 0x081c0196 T _ZN22DisPatcher_ChangeSkill11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_ChangeSkill/check_error.md 逐块映射）：技能树索引
// [b28d-29] N82/N18b：槽号平移 -0x10->-0x14 + 比较方向折痕（4.4 cmpl mem 直比 vs 4.1 两段），语义集合全等  (ORIG 0x081c0196 insn 31->39)
// ∈{-1,0} 时 msg+0xd==1 → 0x13；其它索引时 msg+0xd!=1 → 0x13；否则 0。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 18（ORIG 30 / ours 34），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
int DisPatcher_ChangeSkill::check_error(CUser* user, MSG_BASE& msg)

{
    // [b28d-solo-DisPatcherCore] 删 av/goto 改自然 if/else-if（ORIG 源码形：
    // `cmp $-1; je L_eq` + `test; jne L_ne` 两独立分支、L_ne 侧 `je L_ret0`）。
    const MSG_CHANGE_SKILLSLOT* pReq =
        reinterpret_cast<const MSG_CHANGE_SKILLSLOT*>(&msg);   // ORIG -0x10(%ebp)
    int treeIdx = user->GetCurCharacSkillTreeIndex();          // ORIG -0xc(%ebp)
    // [b28d-solo-DisPatcherCore] 自然 if/else-if 实测 34→29 insn（ORIG 31）：
    // 唯一残差 = ORIG 两次独立 reload `mov -0xc(%ebp),%eax; cmp $0xffffffff,%eax` /
    // `mov -0xc(%ebp),%eax; test %eax,%eax` vs 4.4 折 `cmpl $imm,-0xc(%ebp)` 直比
    // （N18b：4.4 对具名 int 局部比较恒折 mem 直比，无源码写法产两段物化；
    // switch 形实测 30 条且产 add/cmp/ja 树，更劣）⇒ N18b 封顶。
    // 复核（本轮）：P310 形 `(unsigned)treeIdx == K` 与 volatile 形实测均劣化
    // （35 条 / 31 条，ndiff 17/23），原 if/else-if 形 28 条为最优 ⇒ N18b 封顶维持。
    if (treeIdx == -1 || treeIdx == 0)
    {
        if (pReq->m_head0d == 1)
        {
            return 0x13;
        }
    }
    else if (pReq->m_head0d != 1)
    {
        return 0x13;
    }
    return 0;
}
