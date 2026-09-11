// IsSuccess @ 0x849b462：base 成功率 = rate->m_successRate + variation；
// 随机 100 < base 则成功：resultExp ∈ [expMin, expMax]，再加
// GetExpertJobAddtionalExp 的额外经验（概率 m_field0，量 m_field4）。
// [b28d-solo-CExpertJob] 书写序旋钮：`base > get_rand_int(100)` 复现 ORIG
//   0x0849b49c 的 %ebx 免槽跨 call 保活（lea/cmp/setg 段逐条全等）。
// [SweepCExpertJob 2026-09-09] 剩余 1 条 = x87 比较 lowering（4.1
//   `fucompp; fnstsw %ax; test $0x45,%ah; sete` vs 4.4.7 默认
//   `fucomip %st(1),%st; fstp %st(0); seta`），非源码可择；`-march=i486
//   -mtune=generic` 下 4.4.7 直出 ORIG 形 ⇒ AE（93=93 尺寸全等）。
//   p167 CLEAR。ORIG 0x0849b462
bool CExpertJob::IsSuccess(CUser* user, CStackableItem const* item, int& resultExp)
{
    int variation = 0;
    stCompoundRate* rate = GetSuccessRate(user, item, variation);
    if (rate == 0)
        return false;
    // [b28d-62] P0：ORIG 0x0849b49c 把 `rate->m_successRate + variation` 放 %ebx
    //   跨 get_rand_int 保活（无栈槽）⇒ 内联实参，禁具名 base 局部。
    // [b28d-solo-CExpertJob] 书写序旋钮：ORIG `lea (%edx,%eax,1),%ebx; movl
    //   $0x64,(%esp); call; cmp %eax,%ebx; setg` 证明源码左操作数是 base（先物化
    //   %ebx 跨 call 保活）；写成 `get_rand_int(100) < base` 时 4.4 把 call 提到
    //   前面重排（P243/P52 同族：操作数书写序决定求值/装载序）。
    if (rate->m_successRate + variation > get_rand_int(100))
    {
        resultExp = get_rand_int(rate->m_expMax - rate->m_expMin + 1) + rate->m_expMin;
        CInventory* inven = (CInventory*)user->getCurCharacInvenR();
        STItemScript::SEXPERTJOB::stExpertJobAdditionalExp ae;
        // [b28d-62] P0：ORIG 0x0849b51f level 落 %ebx（无槽）⇒ 内联实参。
        inven->GetExpertJobAddtionalExp(
            (ENUM_EXPERT_JOB_TYPE)user->GetCurCharacExpertJobType(),
            user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp()), ae);
        if ((float)get_rand_int(100) < ae.m_field0)
            resultExp += ae.m_field4;
        return true;
    }
    return false;
}