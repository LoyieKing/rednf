int Dispatcher_BuyItem::check_error(CUser* user, CMDPacketStruct::STBaseRequest& req) const
{
    (void)user; (void)req;
    // [objdump 0x0813fbe6] return !isOpenAdvanceAltar();
    // int 返回 + if/else 1/0：复现 ORIG `xor $1` + bool 归一 diamond 形态。
    if (!isOpenAdvanceAltar())
        return 1;
    return 0;
