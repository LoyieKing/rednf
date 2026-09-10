void ARAD::FnRewardMomoji::GetMailString(char* mailBuf, char* titleBuf)
{
    std::strncpy(titleBuf, g_scriptStringManager_.findString(4, "momiji_event_title", (bool*)0), 0x14);
    std::strncpy(mailBuf, g_scriptStringManager_.findString(4, "momiji_event_mail", (bool*)0), 0xff);
