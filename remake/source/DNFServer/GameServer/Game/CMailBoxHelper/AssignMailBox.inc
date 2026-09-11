// ---- 生命周期 / 池 ----
CMailBox* CMailBoxHelper::AssignMailBox()
{
    // ORIG 0x08697838 (W)：g_MailBoxPool.Acquire → reset
    CMailBox* box = g_MailBoxPool.Acquire();
    if (box == 0)
        return 0;
    box->reset();
    return box;
}