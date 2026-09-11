// ORIG 0x0860e838 T：边界检查 <0/>2 直接返回，无空指针分支（P1 数组组件寻址）。
void CCommonStatisticsMgr::IncreaseQuantity(ENUM_STATISTIC_CONTAINER container,
                                            int quantity)
{
    if (container < 0 || container > 2)
        return;
    m_container[container]->IncreaseQuantity(quantity);
}