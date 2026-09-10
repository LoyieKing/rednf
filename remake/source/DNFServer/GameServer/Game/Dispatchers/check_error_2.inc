int Dispatcher_SetSlot::check_error(CUser* user, CMDPacketStruct::STBaseRequest& req) const
{
    (void)user; (void)req;
    if (!isOpenAdvanceAltar())     // [objdump 0x0814004e]
        return 1;
    return 0;
