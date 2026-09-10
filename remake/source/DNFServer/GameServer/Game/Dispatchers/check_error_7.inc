int Dispatcher_ExchangeSlot::check_error(CUser* user, CMDPacketStruct::STBaseRequest& req) const
{
    (void)user; (void)req;
    if (!isOpenAdvanceAltar())     // [objdump 0x08140884]
        return 1;
    return 0;
