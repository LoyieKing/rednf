// ---- respond_special_item_cargo_abs —— ORIG 0x0827bee6 ----
// 绝对容量仓库升级：目标容量 = 当前 GetCapacity()+value；isUpgradable 通过则
// UpgradeCargo 后发包，否则 result.m_value = 0x12。
// b28d-solo-CCeraShop_Special verdict=DIFF [N4] 55/57；余 2 处 lea(%eax,%edx,1) vs add+mov。
// P167 CLEAR 9/9；P248 三档类型旋钮已试（字段/callee 返回类型/同异型）不可择 ⇒ N4/N18a 封顶。
// [CCeraShop_Special 2026-09-11 本轮复核] 另试两形均无效（ndiff 4 不变）：
// ① 删语句表达式（实参写直接表达式）、② `(int)` 内/外层 cast 到 int。
// 根因已定位（报告项，需改 .h）：`CCargo::GetCapacity()` 声明返回 **unsigned int**
// （CUserCharacInfo.h:262），而 `CCargo::isUpgradable(int)` 形参是 int ⇒ 实参
// unsigned→int 有符号性转换，4.4 恒发 `add %edx,%eax; mov %eax,%ebx`；
// ORIG 0x0827bee6 为 `lea (%eax,%edx,1),%ebx`（无转换直入）。三个后端
// （4.4.4/4.4.6/4.4.7）同形 ⇒ 非版本差异；探针 /tmp/pc.cpp f_a（int+int）逐条复现 ORIG。
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
    if (!static_cast<CUser*>(param.m_user)->getCurCharacCargoR()->isUpgradable(
            ({ static_cast<CUser*>(param.m_user)->getCurCharacCargoR()->GetCapacity() +
               param.m_value; })))
    {
        result.m_value = 0x12;
    }
    else
    {
        static_cast<CUser*>(param.m_user)->getCurCharacCargoW()->UpgradeCargo(
            ({ static_cast<CUser*>(param.m_user)->getCurCharacCargoR()->GetCapacity() +
               param.m_value; }));
        m_owner->_sendCargoUpgradePacket(static_cast<CUser*>(param.m_user), type);
    }
}