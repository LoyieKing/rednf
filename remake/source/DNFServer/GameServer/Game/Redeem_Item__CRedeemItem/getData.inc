void CRedeemItem::getData(char* pData) const
{
    // [b28d-L28 round2 + 2026-09-10 复测] ORIG 0x085f78bc 入口即在 begin() **之前**
    // `mov 0xc(%ebp),%eax; mov %eax,-0x20(%ebp)`（形参→具名局部 pData 副本），
    // 之后每处写点都 `mov -0x20(%ebp),%edx` 重载 ⇒ 原码**有**具名 `char* dst`
    // 局部（P0 槽往返律；旧「删缓存」判词作废）。
    // 循环计数：ORIG 尾测 `mov -0x1c(%ebp),%eax; cmp $0x14,%eax; ja`（两段 +
    // 无符号）⇒ 局部声明 int + 比较处显式 `(unsigned int)` 转换（P310 翻案形），
    // 实测复现两段寄存器形（`unsigned int i` 直写会被 4.4 折成 mem 直比）。
    // [复测读数] 本函数改后 **75=75 gap=0**；残余 = ①`dst+4` 写点 4.4 提前
    // `lea 0x4(%eax),%ebx` 缓存（ORIG `mov -0x20(%ebp),%edx; mov %eax,0x4(%edx)`）
    // ②循环体 RHS(*it) vs LHS 地址求值序（N76 族）③end() 临时槽位互换。
    // `char* volatile dst` 形实测 76 条（+1，volatile 中转）⇒ 已回退。
    char* dst = pData;
    std::list<Inven_Item_Expand_RedeemInfo>::const_iterator it =
        m_redeemList.begin();
    *(int*)(dst + 4) = (int)m_redeemList.size();
    int i = 0;
    for (; (unsigned int)i <= 0x14 && it != m_redeemList.end(); ++it, ++i)
        *(Inven_Item_Expand_RedeemInfo*)(dst + i * 0x44 + 8) = *it;
