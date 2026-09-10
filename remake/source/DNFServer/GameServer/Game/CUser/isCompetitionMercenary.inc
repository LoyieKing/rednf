bool CUser::isCompetitionMercenary() const
{
    // ORIG 0x868e9c0：curCharacR 非空 → FindIndexByCharacNo(m_characNo) →
    // 0xffffffff 早退 → CheckCharacIndex → m_characs[idx].GetCompetitionState()==1
    // ORIG 两次 getCurCharacR call（无具名指针槽，技巧 37）：判空一次、
    // 取 m_characNo 再一次。
    if (getCurCharacR() == 0)
        return false;
    // b28c-L5（P218 应用）：下标局部 unsigned int → **int**。ORIG 末段
    //   `mov -0xc(%ebp),%eax / mov 0x8(%ebp),%edx / add $0x6ef6c,%edx /
    //    mov %eax,0x4(%esp) / mov %edx,(%esp) / call vector::operator[]`
    //   是「实参值先物化、容器基址后算」；4.4 对 size_t 形参的下标恒先算基址并
    //   折 `lea disp(%eax),%edx`（反例 ndiff 4 封顶判词已作废）。下标声明 int
    //   ⇒ 实参走 size_t 转换中转，装载序翻为 ORIG 的值先形（P218 探针 w4 同族）。
    //   与 -1 哨兵比较同时回归 ORIG 的 `cmpl $0xffffffff,-0xc(%ebp); jne` 槽比形。
    int idx = (int)m_mercenaryMgr.FindIndexByCharacNo(
        static_cast<unsigned int>(getCurCharacR()->m_characNo));
    if (idx == 0xffffffff)
        return false;
    if (!m_mercenaryMgr.CheckCharacIndex(static_cast<unsigned char>(idx)))
        return false;
    // 残差（ndiff 4，跨版本 N）：末段 ORIG `mov -0xc(%ebp),%eax; mov 0x8(%ebp),%edx;
    // add $0x6effc,%edx`（4.1 先载实参 RHS + mov+add 折成员基址）vs 4.4
    // `mov 0x8(%ebp),%eax; lea 0x6effc(%eax),%edx`（成员对象地址 lea 单条 + 实参后算）；
    // 同一地址两种物化形，4.1↔4.4 求值顺序+lea/add 等效（probe icm/lea 穷举至 N）。
    return m_mercenaryMgr.m_characs[idx].GetCompetitionState() == 1;
