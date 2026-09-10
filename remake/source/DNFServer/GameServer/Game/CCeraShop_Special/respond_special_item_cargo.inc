// ---- respond_special_item_cargo —— ORIG 0x0827be8a ----
// 仓库扩容：getCurCharacCargoW 上调 UpgradeCargo(value)；成功发 cargo 升级包，
// 失败 result.m_value = 0x17。
// b28d-solo-CCeraShop_Special verdict=IDENTICAL_AE [原 DIFF] P292 UpgradeCargo bool 化 + CUser* 视图 + P59 负极性守卫
void SpecialItemHandler::respond_special_item_cargo(int type,
                                                    const STSpecailItem_Param& param,
                                                    STSpecailItem_Result& result)
{
    // b28d-D42 P0/P52：ORIG 0x0827be8a 无 user/value 具名槽——value 先入 %ebx
    // 跨 getCurCharacCargoW call 保活，user 在成功臂逐点重读 param.m_user
    // （两次 `mov (%eax),%edx`），故写成单条链式表达式。
    // b28d-D42：ORIG 0x0827be8a 为「失败块直落 + jmp 尾、成功块在尾」，
    // 判定 `xor $1,%eax; test %al,%al; je <成功>`（4.1 对 x==1 的 xor 展开）。
    // 4.4 的 if/else 恒 then-first（je 落 else）⇒ 条件取反成 `!= 1` 使失败块直落；
    // 残余 xor vs cmp 为 4.4 折叠差异（N18a 同族，见判词）。
    if (!CERA_PARAM_TYPED(param)->m_user->getCurCharacCargoW()->UpgradeCargo(
            CERA_PARAM_TYPED(param)->m_value))
        result.m_value = 0x17;
    else
        m_owner->_sendCargoUpgradePacket(CERA_PARAM_TYPED(param)->m_user, type);
}

