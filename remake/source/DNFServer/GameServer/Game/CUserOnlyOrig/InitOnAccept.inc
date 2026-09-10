int CUser::InitOnAccept(int socketFd)
{
    m_network.set_socket(socketFd, false);
    m_chState = (ch_state)1;   // 头侧 m_chState 已枚举化（ch_state），显式 cast
    m_winPointBuffer = 0;
    m_packetVerifier.Init();
    m_network.initNetworkBuffer();
    m_tradeCount = 0;

    // game option 尾部区逐字节清零（ORIG 0x70390..0x703b8；CGameOption +0x62c..+0x657）。
    m_gameOption.m_gmQuestFlag = 0;
    m_gameOption.m_forceDropFlag = 0;
    m_gameOption.m_playExpAdd = 0;                     // +0x703a4（movl 直接 dword 写）
    m_gameOption.m_breakAwayDungeonClear = 0;          // +0x703a8（movb 单字节写）
    m_gameOption.m_breakAwayDungeonClearCnt = 0;       // +0x703ac
    m_gameOption.m_breakAwayAccureCera = 0;                             // +0x703b0（§8.3 字段化 m_pad64c→int）
    m_gameOption.m_breakAwayQuestTime = 0;             // +0x703b4
    m_gameOption.m_breakAwayRewardOrder = ENUM_BREAK_AWAY_REWARD_ORDER_0;  // +0x703b8（movl $0 宽写）

    m_multiboxLotteryFailCnt = 0;                      // +0x703bc
    m_breakAway.clear();                               // STBreakAwayRewards::clear W 0x0831d454
    m_breakAwayRewardData = 0;                         // +0x703c0（movb；setBreakAwayRewardData ORIG 0x0831d50c）
    m_usedBreakAwayLuckPoint = 0;                      // +0x703c1

    memset(m_skillCommands, 0, sizeof(m_skillCommands));  // ORIG memset(this+0x703db,0,200)
                                                          // （50×STCommand pack(1)=200B）

    m_premium.m_mailBox = (void*)WongWork::CMailBoxHelper::AssignMailBox();
    if (m_premium.m_mailBox != 0)
    {
        // asm 直接以 *(this+0x71b98) 为 this 调 WongWork::CMailBox::Init()
        ((WongWork::CMailBox*)m_premium.m_mailBox)->Init();
    }

    PacketGuard packet;                                // -0x20
    {
        packet.put_header(0, 1);                       // try 区起点 0864954b
        unsigned char origKey[1024];                   // -0x420（local_424，1024B）
        G_Cipher()->GetOriginalKey(origKey);
        int totalKeyLen = G_Cipher()->GetTotalKeyLength();
        packet.put_binary((char*)origKey, totalKeyLen);

        // [b28d-solo-CUserOnlyOrig P167 真形] ORIG 调用序（0x0864959x 段）：
        // GCE/gcn/strlen → put_int；GCE/gcn/strlen（**第二次**）→ GCE/gcn/put_str
        // ⇒ 源码无 nameLen 局部，put_int/put_str 各自内联 strlen(gcn())（旧具名局部
        // 少 1 GCE+1 gcn+1 strlen，p167 MISSING 各 1）。
        packet.put_int((int)strlen(G_CEnvironment()->get_channel_name()));
        packet.put_str(G_CEnvironment()->get_channel_name(),
                       (int)strlen(G_CEnvironment()->get_channel_name()));

        m_seedFromDate = get_seed_from_date();    // +0x711e8（CUser 本体）
        packet.put_int(m_seedFromDate);

        m_checkSumSeed = get_rand_int(0);                      // +0x796e0
        packet.put_int(m_checkSumSeed);

        packet.put_byte(G_CEnvironment()->m_serverEnvir.m_serverGroup);      // env+0x378
        packet.put_byte(G_CEnvironment()->m_serverEnvir.m_channelNo);        // env+0x1b0
        packet.put_byte(G_CEnvironment()->m_serverEnvir.m_processSequence);  // env+0x1a8

        packet.put_int(GlobalData::s_systemTime_.getCurSec());

        int stunCnt = (int)G_CEnvironment()->m_serverEnvir.m_stunIP.size();  // env+0x360
        packet.put_int((int)G_CEnvironment()->m_serverEnvir.m_stunIP.size());
        if (stunCnt != 0)
        {
            // [b28d-solo-CUserOnlyOrig P167] ORIG 循环上界用已缓存 stunCnt（不再调
            // size()），循环体两处完整内联 `GCE()->m_stunIP[i].m_ip`（无 ip 局部缓存）。
            // [A06 2026-09-10 轴B（A32 清单 a 栏；地址自校：ORIG 0x0864975f
            // `addl $1,-0xc; mov -0xc,%eax; cmp -0x10,%eax; setl` 有符号）] 旧 unsigned 索引
            // ⇒ 4.4 `seta`；改 int 直用 stunCnt 以对齐 ORIG 符号性。
            for (int i = 0; i < stunCnt; ++i)
            {
                packet.put_int((int)strlen(
                    G_CEnvironment()->m_serverEnvir.m_stunIP[i].m_ip));
                packet.put_str(
                    G_CEnvironment()->m_serverEnvir.m_stunIP[i].m_ip, 0x10);
            }
            packet.put_int(G_CEnvironment()->m_serverEnvir.m_stunPort[0]);   // env+0x36c
            packet.put_int(G_CEnvironment()->m_serverEnvir.m_stunPort[1]);
        }
        packet.put_int(0x25e);
        packet.put_int(0x232);
        packet.finalize(true);
    }
    Send(packet);

    StartRecvEventWaiting();
    setStdDropRate(100);

    m_postChecksumData.m_tradePunishType = 0;          // +0x79628
    m_humanCertifyTimerKey = 0;                        // +0x8e0f8
    m_humanCertifyErrorCnt = -1;                       // +0x8e0fc
    m_humanCertified = false;                          // +0x8e100

    m_mcap.m_secuUserAction = new cSecuUserAction();
    m_mcap.m_secuUserAction->Init();

    m_pad8e101[0x2eb] = 0;                             // ORIG movb $0x0,0x8e3ec（pad 末字节）
    m_field8e3ed = true;                               // +0x8e3ed
    memset(m_pad8e101, 0, 0x145);                      // memset(this+0x8e101,0,0x145)
    memset(m_pad8e101 + 0x145, 0, 0x1a6);              // memset(this+0x8e246,0,0x1a6)
    m_mcap.reset();                                    // WongWork::CMCAPManager::reset W 0x08694fa0
    m_postChecksumData.m_field31 = false;              // +0x7962d

    getSecurityCard()->init();                         // thunk 0x0822fd20/WongWork::CSecurityCard::init 0x085fdd68
    getPad()->init();                                  // Sanicova::CPad::init 0x0859957c
    m_mcap.m_field1c = 0;                              // +0x8e40c（GetAutoClientRate 清零）

    // gm 调试角色（函数级 static，guard/data 名与 ORIG _ZGVZN5CUser12InitOnAcceptEiE…同版式）
    static CGameMasterCharacter gameMasterCharacter;
    m_postChecksumData.m_gameMasterCharac = &gameMasterCharacter;   // +0x7964c

    m_mcap.m_capchaReqTick = 0;                        // +0x8e94c
    m_mcap.m_field544 = false;                         // +0x8e934
    m_mcap.m_hackIpLogged = false;                     // +0x8e950
    m_mcap.m_field561 = false;                         // +0x8e951

    m_hades.m_secuReward = false;                      // +0x8e97c
    m_hades.m_secuType = 0;                            // +0x8e97d
    // [b28d-solo-CUserOnlyOrig P167] ORIG 0x08649a0x `call getHades; call CHades::Init`
    // ⇒ 源码经 getHades() 取址（p167 MISSING getHades ×1）。
    getHades()->Init(this);                            // XNuclear::CHades::Init T 0x084b91e8
    m_goldControl.Init(this);                          // Secu_GoldControl::Init T 0x08287230
    m_hackLogCheck.Init(this);                         // Secu_HackLogCheck::Init T 0x0827a7d0
    m_mcap.m_field546 = 0;                             // +0x8e936（short）
    m_mcap.m_lastLotteryTime = 0;                      // +0x8e938
    m_mcap.m_loadingHackGold = false;                  // +0x8e944（bool 字段，ORIG movb $0x0 直写）
    m_piSender.m_nonClientFlag = 0;                    // +0x8eaf4
    m_piSender.m_nonClientRandInt = 0;                 // +0x8eaf8
    m_debugCommand = 0;                                // +0x8e090
    m_ceraUserInfo.m_updateOntime = false;             // +0x8d24c（UserInfo+0x68 bool）
    m_accHacking.init(this);                           // Secu_AccountHacking::init T 0x08278a98
    m_gameOption.m_forceDropFlag = 0;                  // +0x70391（第二处 movb $0x0）
    memset(&m_garenaAuthData, 0, sizeof(m_garenaAuthData));  // ORIG memset(this+0x6ef92,0,0x68)（InitOnAccept.md:589）

    return 1;                                          // asm mov $0x1,%ebx → %eax 返回 1
