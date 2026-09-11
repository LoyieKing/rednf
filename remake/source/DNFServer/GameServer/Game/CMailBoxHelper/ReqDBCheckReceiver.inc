// ---- 收件人校验 ----
int CMailBoxHelper::ReqDBCheckReceiver(CUser* user, const char* name,
                                       MSG_CHECK_MAIL_RECEIVER* msg)
{
    // ORIG 0x08555464：直接转发 forchina 版本；帧 0x28（P31 未用局部占位）
    char frame_pad[0x10];
    (void)frame_pad;
    return ReqDBCheckReceiver_forchina(user, name, msg);
}