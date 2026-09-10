int Dispatcher_ResetStar::check_error(CUser* user, CMDPacketStruct::STBaseRequest& req) const
{
    (void)user; (void)req;
    if (!isOpenAdvanceAltar())     // [objdump 0x0814108a]
        return 1;
    return 0;
