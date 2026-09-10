void CUser::ResetItemByScript(std::vector<std::pair<int, int> >& list)
{
    // 残差 P75/N80 族（跨版本实参装载中转寄存器差）：两处 operator[](i) 的
    // 实参 i 装载 ORIG 4.1 经 %edx 中转入 0x4(%esp)，4.4 恒 %eax。求值序/槽位/
    // 调用全同，仅中转寄存器名差，源形不可复现。
    // p167 差集（b28）：ORIG 走 `operator[](uint) const`×2 + make_pair<int const&,int>
    // ⇒ 原码以 const 指针访问（容器 cv 限定决定实例化，P96）。
    // b28d-solo-CUser 三形复测（2026-09-09）：显式 `items->operator[](i)` /
    // `unsigned int i` / const 引用 `items[i]` 三形均不翻 ORIG 的「this 先装
    // -0x14→%eax、i 后装 -0xc→%edx」实参序（4.4 恒值先）⇒ 维持 P75/N80 封顶。
    const std::vector<stResetItemInfo::ResetItem>* items =
        G_CDataManager()->GetResetItemInfo();
    int now = GlobalData::s_systemTime_.getCurSec();
    for (size_t i = 0; i < items->size(); ++i)
    {
        if ((*items)[i].m_resetSec <= now)
            list.push_back(std::make_pair((*items)[i].m_space, 0));
    }
