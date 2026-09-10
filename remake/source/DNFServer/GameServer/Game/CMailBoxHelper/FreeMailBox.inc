void CMailBoxHelper::FreeMailBox(CMailBox* box)
{
    // ORIG 0x0869786a (W)
    g_MailBoxPool.Free(box);
