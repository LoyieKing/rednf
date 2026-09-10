int Inter_GetUserRegdate::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_GetUserRegdatePktView* pPkt = reinterpret_cast<Inter_GetUserRegdatePktView*>(data);
    // ORIG +0x11/+0x17：`mov 0x8(%eax),%edx`/`mov 0x4(%eax),%eax` 单级带偏移读 ⇒ 字段直取
    // （int* 中间槽会退化成 add+mov 两级）。
    checkNewMemberHacklog(pPkt->m_4, user, pPkt->m_8);
    return 0;
