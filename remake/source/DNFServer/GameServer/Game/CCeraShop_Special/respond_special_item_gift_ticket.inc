// ORIG 仅读取 param 的 m_user/m_value 到局部后返回（O0 保留死读取）。
void SpecialItemHandler::respond_special_item_gift_ticket(int,
                                                          const STSpecailItem_Param& param,
                                                          STSpecailItem_Result&)
{
    void* user = param.m_user;
    int value = param.m_value;
}