int Dispatcher_SDC_ActiveStatus_Damage_Check::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081ea16e（docs/class_func_reports/Dispatcher_SDC_ActiveStatus_Damage_Check/dispatch_sig.md）
    unsigned char count = 0;                  // local_59 -0x55
    SDCDamageSyncView* data;                  // local_44/48
    unsigned long destLen;                    // local_5c -0x5c
    unsigned long srcLen;                     // local_34 -0x34
    int uncompreRet;                          // local_30 -0x30
    int clientDamage;                         // local_2c -0x2c
    int clientDuration;                       // local_28 -0x28
    // ORIG 0x081ea3d1/0x081ea3d8：先 -0x64（svrDamage）后 -0x68（svrDuration）初始化。
    int svrDamage = 0;                        // local_64[0] -0x64（outB）
    int svrDuration = 0;                      // local_68 -0x68（outA）
    unsigned char i;                          // local_3d -0x3d
    int ret;                                  // local_30 复用
    int adjDamage;                            // local_1c -0x1c
    CParty* party;                            // local_20 -0x20

    (void)uncompreRet;

    // +0x0c：读条目数
    if (!packet.get_byte(count))
        return LineFunc(0x6b36,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0x59：ORIG 四个 500 上限初始化（-0x3c/-0x38 为 dead，-0x5c=destLen、
    // -0x34=srcLen），随后两缓冲内联 rep stos 慢路清零（P214③/P312）。
    int deadLen1 = 500;                       // ORIG -0x3c（dead）
    int deadLen2 = 500;                       // ORIG -0x38（dead）
    destLen = 500;                            // ORIG -0x5c
    srcLen = 500;                             // ORIG -0x34
    // P312（实测）：前置 1 字节**数组**死局部把大数组顶到非 4 对齐槽
    // （ORIG src@-0x25d/dest@-0x451 均 ≡3 mod 4 ⇒ 4.1 发对齐剥离慢路；
    //  probe /tmp/l1/probe1.cpp cPad1 形态 4.4 逐条复现 36 条形）。
    char alignPad1[1];
    char src[500] = {0};                      // ORIG -0x25d（压缩数据）
    char alignPad2[1];
    char dest[500] = {0};                     // ORIG -0x451（解压数据）
    // P332：`(void)pad;` 哑引用**不占槽**（实测 4.4 忽略）⇒ 数组回落 4 对齐、
    // 慢路消失；此处用真实写入让 1 字节数组占槽，把下游大数组顶到 ≡3 mod 4。
    alignPad1[0] = 0;
    alignPad2[0] = 0;
    (void)deadLen1;
    (void)deadLen2;
    data = (SDCDamageSyncView*)dest;

    // +0x134：逐条目（byte 计数，setb 无符号比较）
    for (i = 0; i < count; i = (unsigned char)(i + 1))
    {
        int compressedSize = 0;               // local_64[0] 复用槽（get_int 目标）
        // +0x13d：读压缩长度
        if (!packet.get_int(compressedSize))
            return LineFunc(0x6b4a,
                            __PRETTY_FUNCTION__,
                            0, 0);
        // +0x186：> 500 → 拒绝
        if (500 < compressedSize)
            return LineFunc(0x6b4d,
                            __PRETTY_FUNCTION__,
                            0, 0);
        // +0x1b9：读压缩数据
        if (!packet.get_binary(src, compressedSize))
            return LineFunc(0x6b50,
                            __PRETTY_FUNCTION__,
                            0, 0);
        // +0x205：zlib 解压（ORIG 循环内**不**重置 destLen）
        srcLen = (unsigned long)compressedSize;    // ORIG 0x081ea376
        ret = uncompress((unsigned char*)dest, &destLen,
                         (unsigned char*)src, srcLen);
        if (ret != 0)
            return LineFunc(0x6b59,
                            __PRETTY_FUNCTION__,
                            0, 0);

        // +0x263：客户端上报值
        clientDamage = data->m_damage;
        clientDuration = data->m_duration;
        svrDuration = 0;
        svrDamage = 0;
        SDCActiveStatusSyncServer syncServer;                         // ctor 0823487c
        syncServer.getActiveStatusDamageDurationForSDC(
            (char)data->m_event, (int)(unsigned int)(unsigned char)data->m_eventKind,
            &svrDuration, &svrDamage,
            (STSDCActiveStatusSyncDataSend*)data, 0);
        // +0x2d9：不匹配 → 黑客计数 + trace
        if (svrDamage != clientDamage || svrDuration != clientDuration)
        {
            user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x25d, 1, 0, 0);
            {
                cMyTrace trace(
                    __PRETTY_FUNCTION__,
                    0x6b6d, 0);
                trace("[TYPE : %d][EVENT : %d] [Svr] Damage : %d, Duration : %d, [Client] Damage : %d, Duration : %d",
                      (int)(char)data->m_event, (int)(char)data->m_type,
                      svrDamage, svrDuration, clientDamage, clientDuration);
            }
        }
        // +0x3a0：队伍战场伤害记录
        party = G_CGameManager()->GetParty((int)user->GetPartyIndex());
        if (party != 0)
        {
            if (data->m_event == 0xb)
            {
                // +0x3de：流血事件 → 伤害按 ORIG 全局 BLEEDING_DAMAGE_IN_ACTION_RATE 缩放
                // ORIG 0x081ea558：fildl/flds/fmulp/fnstcw/fldcw(RC=trunc)/fistpl ⇒ 直接 C cast
                adjDamage = (int)(BLEEDING_DAMAGE_IN_ACTION_RATE * (float)clientDamage);
            }
            else
            {
                adjDamage = clientDamage;
            }
            CBattle_Field* battleField = &party->m_battleField;   // CParty +0xb24
            battleField->SetDamagedPacketCnt(data->m_cnt);
            battleField->SetDamageValue(data->m_cnt, adjDamage);
        }
        // syncServer 析构（08234890）
    }
    return 0;
