// ============================================================================
// importInGameAdvertisementEtcScript — ORIG 0x0897e9c4（init.md +0x048c，
// "Etc/ingame_advertisement.etc" → m_inGameAd）。clear() 将前 151 字节置 1；
// "[in game advertisement expose condition of teen]" 后每个 idx（ScanInt
// 成功且 <=150）清 0 一字节，idx>150 结束本段。
// ============================================================================
// [b28d-solo-InitScriptProducers_7] verdict=AE（已修）ORIG 0x0897e9c4
//   P164 分离形 `while (run) { if (!ScanType(token, true)) break; }`（条件头
//   仅测 run、ScanType 检查落体内首）。
bool importInGameAdvertisementEtcScript(InGameAdvertisementScript* s, const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    InGameAdvertisementScript* p = s;          // ORIG -0x14 arg 缓存
    s->clear();                                // ORIG：p 存槽先于 clear call

    std::string token;                         // ORIG -0x18
    bool run = true;                           // ORIG -0xd（token ctor 后写 1）
    int idx = 0;                               // ORIG -0xc（声明序 run→idx→ok，[b28d-47]）
    bool ok = false;                           // ORIG -0x19

    // [b28d-solo-InitScriptProducers_7] ORIG 0x0897e9c4 分离形：条件头仅测 run
    //（cmpb $0,-0xd; jne 体），ScanType 检查落体内首（call;xor$1;test;jne nop 岛）
    // —— 合并 `while (run && ScanType(...))` 在 4.4 产短路段物化 + je 体，形异。
    while (run)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[in game advertisement expose condition of teen]")
        {
            for (;;)
            {
                idx = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                if (idx > 150)
                {
                    break;
                }
                p->m_exposeConditionOfTeen[idx] = 0;   // ORIG movb$0,(edx,eax)
            }
        }
    }
    return true;
}