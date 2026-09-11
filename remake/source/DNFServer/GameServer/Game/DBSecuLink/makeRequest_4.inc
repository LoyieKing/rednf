// ----------------------------------------------------------------------------
// DB_SecurityCardActivate::makeRequest(uint uid, uint accId, certKey, activated)
// ORIG @ 0x0842bb5c（报告 DB_SecurityCardActivate/makeRequest.md）：
// Acquire("DBThread.cpp", 0x6e99) → << 0xff（op<<(int)）| << uid(int) →
// GetInBuffer<SIG_SECURITY_CARD_ACTIVATE>()（0xc 字节，无 memset）填：
//   +0x00 = accId（uint）；strncpy(+0x04, certKey, 6)；+0x0b(byte) = activated。
// → put(DB_Q)。
// ----------------------------------------------------------------------------
void DB_SecurityCardActivate::makeRequest(int uid, unsigned int accId,
                                          char const* certKey, bool activated)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6e99), true);

    **guard << 0xff;                    // op<<(int)
    **guard << uid;                     // op<<(int)
    {
        SIG_SECURITY_CARD_ACTIVATE* p =
            guard->GetInBuffer<SIG_SECURITY_CARD_ACTIVATE>();
        p->m_mId = accId;                        // +0x00（4B mov）
        strncpy(&p->m_certKey[0], certKey, 6);   // +0x04（&x[0] 同上，保 ORIG lea 形）
        p->m_historyFlag = activated;            // +0x0b 1B（ORIG 直接存 bool，movzbl/mov %dl）
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}