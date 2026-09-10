bool CCeraShop::CallSpecialItemHandler(int type, const STSpecailItem_Param& param,
                                       STSpecailItem_Result& result)
{
    return m_handler->execute(type, param, result);
