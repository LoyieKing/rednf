bool CFairPvPScore::loadData(CUser* pUser, char* pData)
{
    SIG_FAIR_PVP_SCORE* sig = reinterpret_cast<SIG_FAIR_PVP_SCORE*>(pData);
    int idx = 0;

    // 区块 1：职业计分表（entry stride 0x10，起于 (idx+2)*0x10）
    // ORIG 084a491d..084a4a45：count 每轮重读 sig->m_jobScoreCount（无局部）；
    // key 为 make_pair 实参临时（mangled make_pair<JobKey,COUNT&> 实证）；
    // 第二实参直接引用线格式计数（`lea 0x14(%eax),%ebx`），旧版复制到局部 value
    // 会多 3 store 且丢失引用语义。
