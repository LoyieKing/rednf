// ORIG 0x0822284a T _ZN39Dispatcher_FinishVillageMonsterFighting12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/Dispatcher_FinishVillageMonsterFighting/dispatch_sig.md 汇编+反编译 C 全文转写）
// ORIG G_CEnvironment()+0x1a8 = m_serverEnvir.m_serverType（Server_Envir 基址 +0x198 + 0x10）。
// b28d-solo-Dispatchers verdict=AE(routed) [S162-verify] ORIG 0x0822284a orig/ours=70/70
int Dispatcher_FinishVillageMonsterFighting::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        return 0;
    }
    // 【batch-17 S池 P48/P183】ORIG 0x0822286b/0x0822287b：**两臂各调一次**
    // G_CEnvironment 再读 +0x1a8，缓存 serverType 少 1 call。
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 1 ||
        G_CEnvironment()->m_serverEnvir.m_serverType == 2)
    {
        return 0;
    }
    if (user == 0)                 // ORIG +0x05b：cmpl $0,0xc(%ebp)（user，非 &packet）
    {
        return LineFunc(0xdc66,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->get_state() != 5)
    {
        return 0;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0)
    {
        int result = 0;                                        // ORIG movl $0,-0xc（先零初始化）
        result = party->OnFinishVillageMonsterFighting(user, false);
        // [b28c-34] ORIG 0x08222931 `cmpl $0,-0xc; setne %al; test %al,%al; je`
        // = P225 四段物化：源形 while(v!=0){body;break;} 精确复现（if 形恒折直跳）。
        while (result != 0)
        {
            user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x10c),
                                     static_cast<unsigned char>(result));
            break;
        }
    }
    return 0;
}