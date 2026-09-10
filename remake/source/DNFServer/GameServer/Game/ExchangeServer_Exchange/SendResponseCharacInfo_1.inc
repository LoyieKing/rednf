bool CExchangeServer::SendResponseCharacInfo(int a, int b, void* data,
                                             int len)
{
    // P190 撑帧（同 ForwardRequestCharac）：双 double 取址 pad 使 4.4 帧 0x28→0x38。
    double framePad1, framePad2;
    (void)&framePad1;
    (void)&framePad2;
    return SendResponseCharacInfo_forchina(a, b, data, len);
