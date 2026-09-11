// ---- respond_special_item_booster_selection —— ORIG 0x0827ccfc ----
// 转发 CCeraShop::_processBoosterSelect（参数映射见报告汇编）。
void SpecialItemHandler::respond_special_item_booster_selection(
    int, const STSpecailItem_Param& param, STSpecailItem_Result& result)
{
    // b28d-solo-CCeraShop_Special verdict=DIFF [N82] 63/63 等长；P0 九具名局部复现
    // ORIG 槽序（-0x34..-0x19），P167 CLEAR 1/1，余帧 0x5c/0x50 + m_flag bool 归一化。
    // b28d-solo(P0)：ORIG 0x0827ccfc 九个实参各自占具名栈槽（-0x34..-0x19，
    // 逐字段 `mov 0x10(%ebp),%eax; mov disp(%eax),%eax; mov %eax,-N(%ebp)`），
    // 声明序 = ORIG 槽序 user/itemIdx/items/ceraParam/count/time/flag/c1/c2。
    // [L14] 判据⑩ 攻法实测（回退）：头声明 `char framePad[0x10]` 使帧 0x50→0x60
    // （P232 生效），但九个具名标量槽号仍 -0x24..-0x9 **不动**（4.4 把数组 pad
    // 另置，不入标量块）⇒ 整块平移不可由 pad 修正，判 N82附。
    CUser* user = static_cast<CUser*>(param.m_user);
    int itemIdx = param.m_itemIdx;
    std::vector<Inven_Item>* items =
        static_cast<std::vector<Inven_Item>*>(param.m_items);
    WongWork::CCeraShop::stCeraShopItemParam_t* ceraParam =
        static_cast<WongWork::CCeraShop::stCeraShopItemParam_t*>(param.m_ceraParam);
    int count = param.m_count;
    long time = param.m_time;
    // [L14+/H9 批次3] ORIG 0x0827cd3d `movzbl 0x1c(%eax),%eax; mov %al,-0x1b(%ebp)`
    // （直存字节）+ 0x0827cd60 `movzbl -0x1b(%ebp),%ebx`（传 bool 形参，**无**
    // `cmpb $0; setne` 归一化）。uchar 局部在 4.4 下传 bool 形参必插 setne
    // （实测 +3 条），只有 bool←bool 直存 + bool 直传才复现 ORIG ⇒ 局部改 bool
    // （P292/T3；字段侧 m_flag 已由 H9 批次2 改 bool，sizeof/偏移不变）。
    bool flag = param.m_flag;
    char c1 = static_cast<char>(param.m_char1d);
    char c2 = static_cast<char>(param.m_char1e);
    m_owner->_processBoosterSelect(user, itemIdx, count, result.m_value, time,
                                   flag, *items, *ceraParam, c1, c2);
}