// ---------------------------------------------------------------------------
// CallSpecialItemHandler —— ORIG T 0x08325854 (0x2a)
// 薄转发：m_handler->execute(type,param,result)。
// ---------------------------------------------------------------------------
bool CCeraShop::CallSpecialItemHandler(int type, const STSpecailItem_Param& param,
                                       STSpecailItem_Result& result)
{
    return m_handler->execute(type, param, result);
}