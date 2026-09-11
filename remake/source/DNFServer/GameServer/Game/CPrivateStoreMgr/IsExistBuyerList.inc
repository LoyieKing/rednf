// ---- IsExistBuyerList（ORIG T 0x85c91b6，报告 IsExistBuyerList.md L15-91 全文）----
// [b28d-24] DIFF 残24: 迭代器临时槽阶梯/构造顺序（P28/P50）。ORIG 0x085c91b6
bool CPrivateStore::IsExistBuyerList(CUser* user)
{
    // P56：ORIG 尾部 test%al;je;mov$1;jmp;mov$0 为 if/return 分支形，勿直返表达式。
    // 具名迭代器固定 find 先求值（P74 补充②旋钮一），复现 ORIG 的
    // end(-0x14)/begin(-0x10)/find(-0x18)/end(-0xc) 槽阶梯。
    std::list<CUser*>::iterator found = std::find(m_users.begin(), m_users.end(), user);
    if (found != m_users.end())
        return true;
    return false;
}