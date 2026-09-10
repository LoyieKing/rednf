void DB_SecurityCardRetransfer::makeRequest(
    int type, char* characName, char* targetName, unsigned int secCardKey,
    char const* src1, char const* src2, char const* src3)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6f38), true);
    **guard << (int)0x138;
    **guard << (int)type;
    SIG_SECURITY_CARD_RETRANSFER* sig = guard->GetInBuffer<SIG_SECURITY_CARD_RETRANSFER>();
    // 共享头 SIG_SECURITY_CARD_RETRANSFER 仅 m_pad[0x8c]（其它 TU 亦各持局部
    // 视图）；§8.3 本 TU 具名视图，偏移/宽度证据=ORIG makeRequest @0x0842c28a
    // ..0x0842c309 各 strncpy 立即数。
    struct SecurityCardRetransferView
    {
        unsigned int m_secCardKey;   // +0x00
        char m_src2[0x21];           // +0x04
        char m_src1[6];              // +0x25
        char m_src3[0x27];           // +0x2b
        char m_characName[0xc];      // +0x52
        char m_targetName[0x29];     // +0x5e
        char m_padTail[5];           // +0x87..+0x8b
    };
    static_assert(sizeof(SecurityCardRetransferView) == 0x8c,
                  "SecurityCardRetransferView size");
