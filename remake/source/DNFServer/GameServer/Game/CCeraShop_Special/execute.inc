// ---- SpecialItemHandler::execute —— ORIG 0x0827be0a ----
// 表驱动分发：type<0xe 时取 m_entries[type] 的 respond PMF 调用。
// ORIG 用 4.1 PMF 调用序列（ptr bit0 虚判定 + adj this 调整）；本对象所有条目
// 均为非虚成员函数（adj=0、ptr bit0=0），itoc 直接路径。
// b28d-solo-CCeraShop_Special verdict=IDENTICAL_AE [原 DIFF] 真实 PMF（SpecialItemHandler.h Entry::fn）复现 4.1 itoc 调用序列
bool SpecialItemHandler::execute(int type, const STSpecailItem_Param& param,
                                 STSpecailItem_Result& result)
{
    if (type > 0xd)
        return false;
    (this->*m_entries[type].fn)(type, param, result);
    return true;
}