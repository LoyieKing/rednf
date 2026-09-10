bool CDBAndExchangeServerProc::RequestLoadActionPointFromDBAndES(CUser&, MSG_CHARAC_SELECT&)
{
    // [b28c-4] ORIG 0x08121608：sub $0x10 + mov $1,%eax ⇒ 空体带未使用局部帧。
    char framePad[0x10];
    (void)framePad;
    return true;
