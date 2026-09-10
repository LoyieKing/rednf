// ---- respond_special_item_cargo_abs —— ORIG 0x0827bee6 ----
// 绝对容量仓库升级：目标容量 = 当前 GetCapacity()+value；isUpgradable 通过则
// UpgradeCargo 后发包，否则 result.m_value = 0x12。
// b28d-solo-CCeraShop_Special verdict=DIFF [N4] 55/57；余 2 处 lea(%eax,%edx,1) vs add+mov。
// P167 CLEAR 9/9；P248 三档类型旋钮已试（字段/callee 返回类型/同异型）不可择 ⇒ N4/N18a 封顶。
void SpecialItemHandler::respond_special_item_cargo_abs(int type,
                                                        const STSpecailItem_Param& param,
                                                        STSpecailItem_Result& result)
{
    // b28d-D42 verdict=DIFF [N18a+N82] ORIG 0x0827bee6 orig/ours=55/57 ndiff=39。
    // P0/P52 去具名槽后余 ORIG 两段 `mov mem,%eax; mov %eax,%ebx` 中转被 4.4
    // 折单段（N18a）+ 帧差；语义等价封顶。
    // b28d-D42 P0/P52：ORIG 0x0827bee6 无 user/value 具名槽——param.m_value
    // 从载荷直读入 %edx 与 capacity 相加后存 %ebx 跨 call 保活；user 逐点重读
    // `mov 0x10(%ebp),%eax; mov (%eax),%eax`（P48 无 CSE）。
    if (!CERA_PARAM_TYPED(param)->m_user->getCurCharacCargoR()->isUpgradable(
            ({ CERA_PARAM_TYPED(param)->m_user->getCurCharacCargoR()->GetCapacity() +
               CERA_PARAM_TYPED(param)->m_value; })))
    {
        result.m_value = 0x12;
    }
    else
    {
        CERA_PARAM_TYPED(param)->m_user->getCurCharacCargoW()->UpgradeCargo(
            ({ CERA_PARAM_TYPED(param)->m_user->getCurCharacCargoR()->GetCapacity() +
               CERA_PARAM_TYPED(param)->m_value; }));
        m_owner->_sendCargoUpgradePacket(CERA_PARAM_TYPED(param)->m_user, type);
    }
}

