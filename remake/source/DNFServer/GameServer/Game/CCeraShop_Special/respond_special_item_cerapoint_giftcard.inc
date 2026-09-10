// ---- respond_special_item_cerapoint_giftcard —— ORIG 0x0827de4e ----
// ORIG：经 GlobalData::s_pIPGHelper 虚表 +0xc 调用
//   (helper, user, value, 2, 0, 0)。
// b28d-solo-CCeraShop_Special verdict=IDENTICAL_AE [原 DIFF] P254 enum 视图 STSpecailItem_ParamValueEnumView 复现两段装载
void SpecialItemHandler::respond_special_item_cerapoint_giftcard(
    int, const STSpecailItem_Param& param, STSpecailItem_Result&)
{
    // ORIG 0x827de4e：vtable+0xc 载入→%ebx 跨实参求值存活（P52 单表达式形）。
    // IPGHelper 该槽未建模，保留裸槽派发。
    // [N7 封顶] 单表达式后 4.4 分配 vtable 项→%eax、value→%ebx、user→%ecx，
    // ORIG 为 ebx/ecx/edx——纯寄存器名互换（N7 不可翻）；帧 0x24+push%ebx 已复现。
    // b28d-D42 P265：ORIG 0x0827de4e 的 param.m_value 装载为两段
    // `mov 0x8(%eax),%eax; mov %eax,%ecx`——按 P265 在实参处显式 cast 到 enum
    // 复现该中转（int 字段 + enum 实参 ⇒ 4.4 保留 eax 中转）。
    (*reinterpret_cast<void (***)(void*, void*, int, int, int, int)>(
        GlobalData::s_pIPGHelper))[3](
        GlobalData::s_pIPGHelper,
        CERA_PARAM_VALUE_ENUM(param)->m_user,
        CERA_PARAM_VALUE_ENUM(param)->m_value,
        2, 0, 0);
}

