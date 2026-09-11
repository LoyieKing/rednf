// ----------------------------------------------------------------------------
// DB_SecurityCardIssue::makeRequest(uint uid, uint accId, certKey, serverKey,
//                                   phone, sha1Hex, cardData)
// ORIG @ 0x0842b57a（报告 DB_SecurityCardIssue/makeRequest.md）：
// Acquire("DBThread.cpp", 0x6e0a) → << 0xfd（op<<(int)）| << uid(int) →
// GetInBuffer<SIG_SECURITY_CARD_ISSUE>()（0x90 字节，无 memset）填：
//   +0x00 = accId（uint）；strncpy(+0x31, certKey, 6)；strncpy(+0x04, serverKey,
//   0x21)；strncpy(+0x25, phone, 0xc)；strncpy(+0x37, cardData, 0x27)；
//   strncpy(+0x63, sha1Hex, 0x29)。→ put(DB_Q)。
// 注意 ORIG 汇编 strncpy 次序与反编译一致（cardData@+0x37 先于 sha1Hex@+0x63）。
// ----------------------------------------------------------------------------
void DB_SecurityCardIssue::makeRequest(int uid, unsigned int accId,
                                       char const* certKey, char const* serverKey,
                                       char const* phone, char const* sha1Hex,
                                       char const* cardData)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6e0a), true);

    **guard << 0xfd;                    // op<<(int)
    **guard << uid;                     // op<<(int)

    {
        SIG_SECURITY_CARD_ISSUE* p =
            guard->GetInBuffer<SIG_SECURITY_CARD_ISSUE>();
        p->m_mId = accId;                          // +0x00（4B mov）
        // &x[0] 显式取首元素：union 包裹成员的数组衰减在 gcc4.4 生成
        // `addl $off,%eax`（原地破坏寄存器存活），ORIG（gcc4.1，plain 视图）
        // 为 `lea off(%eax),%edx`；&x[0] 形态恢复同形（/tmp/leatest2 实证）。
        strncpy(&p->m_certKey[0], certKey, 6);         // +0x31
        strncpy(&p->m_serverKey[0], serverKey, 0x21);  // +0x04
        strncpy(&p->m_serial[0], phone, 0xc);          // +0x25
        strncpy(&p->m_card[0], cardData, 0x27);        // +0x37
        strncpy(&p->m_phone[0], sha1Hex, 0x29);        // +0x63
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}