int Inter_UpdateMiniCraneSeed::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_UpdateMiniCraneSeedPktView* pPkt = reinterpret_cast<Inter_UpdateMiniCraneSeedPktView*>(data);
    // ORIG +0x11 一步 `mov 0xa(%eax),%ebx`（同 signedness 直落 callee-saved）⇒ register 局部；
    // 且无多余 char* 局部（否则 pPkt 槽被挤到 -0x14/-0x10 两级）。
    register int seed = pPkt->m_seed;
    G_GameWorld()->ResetCraneItemNeedMaterial(seed);
    return 0;
}