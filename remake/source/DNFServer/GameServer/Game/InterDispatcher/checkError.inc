// ORIG 0x084e6acc T _ZN25Inter_NpcLimitBuyItemInfo10checkErrorEP5CUserP19LimitNpcBuyItemInfo
// 报告 docs/class_func_reports/Inter_NpcLimitBuyItemInfo/checkError.md：
// 用户/状态/角色号校验→find_item→GetNeedMaterial 填 m_needItem→
// GetInvenData(this+0x5a)→add_info/计数比较，返回错误码。
// [b28d-lane2] verdict=DIFF ndiff=96（ORIG 0x084e6acc）：寄存器中转(N18a/N7)+混合残差；gap-4。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=64（ORIG 0x84e6acc）：N22 cmp 操作数方向+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_NpcLimitBuyItemInfo::checkError(CUser* user, LimitNpcBuyItemInfo* info)
