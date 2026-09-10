// ---- respond_special_item_inventory_upgrade —— ORIG 0x0827bf90 ----
// 背包容量 < 0x10 时：ExpandInventory 日志 → 容量 += value → DB_UpdateInventoryCapacity
// 持久化 → _sendInventoryUpgradePacket；否则 result.m_value = 0x12。
// b28d-solo-CCeraShop_Special verdict=NEAR [N7] 92/92 等长，仅 makeRequest 三 callee-saved 寄存器角色轮换
void SpecialItemHandler::respond_special_item_inventory_upgrade(
    int type, const STSpecailItem_Param& param, STSpecailItem_Result& result)
{
    // b28d-D42 verdict=DIFF [N82/N89] ORIG 0x0827bf90 orig/ours=92/92 ndiff=62。
    // 残差为帧/槽号平移 + 日志/STL 内联差；语义按报告逐条对齐。
    CUser* user = static_cast<CUser*>(param.m_user);
    // ORIG 0x827bf90..：getCurCharacInvenR（const 版，无 enableSaveInven 副作用）
    // 与 get_inventory_capacity 各 5+ 处逐点重发（P48/P183 禁缓存）。
    if (user->getCurCharacInvenR()->get_inventory_capacity() > 0xf)
    {
        result.m_value = 0x12;
        return;
    }
    user->m_historyLog.ExpandInventory(
        user->getCurCharacInvenR()->get_inventory_capacity(),
        user->getCurCharacInvenR()->get_inventory_capacity() + param.m_value, 0);
    // ORIG set 处 this 仍是第 5 次 getCurCharacInvenR（const 视图强转，
    // 全函数 InvenR×6 / capacity×5 静态调用点，无 W 版）。
    const_cast<CInventory*>(user->getCurCharacInvenR())->set_inventory_capacity(
        user->getCurCharacInvenR()->get_inventory_capacity() + param.m_value);
    DB_UpdateInventoryCapacity::makeRequest(
        user->GetServerGroup(), user->GetUID(),
        user->getCurCharacNo(),
        user->getCurCharacInvenR()->get_inventory_capacity());
    m_owner->_sendInventoryUpgradePacket(CERA_PARAM_TYPED(param)->m_user, type);
}

