void CUser::_onChangeGrowType(unsigned char newFirst, unsigned char secondGrew,
                              eChangeGrowTypeReason reason)
{
    // [Lane39 判据①/P0/P224] ORIG 0x0866409x 无 ev/on 具名槽：`call GetRepeatEvent` 后直接
    // `mov (%eax),%edx; add $0x34,%edx; mov (%edx),%edx; call *%edx` 且 `test %al,%al;
    // je +0xb4` 与 `cmpl $1,0x14(%ebp); je +0xb4` **两失败出口同目标**（裸短路 &&）；
    // 具名 bool on 反产 `movb %al,-0x29; cmpb $0,-0x29; je` 物化 + 多 jcc/jmp。
    {
        // [Lane39 判据①/P223] ORIG 两个失败出口同落 +0xb4（共享 join）。
        // [A06 2026-09-10 C05③ 两手实测] 负条件 goto 与 `||` 合并（199/207 反劣）均不达标；
        // 按 Main 三手协议第 2 手（守卫含 call/setcc ⇒ **正向嵌套 if**，F3c 实证）改写：
        // 4.4 把 fail 路径 jump-thread 到共享尾出口，逐条 `test; je` / `cmp; je` 与 ORIG 同构。
        if (GlobalData::s_event_manager->GetRepeatEvent(0x15)->IsEventing((CUser*)0))
        {
            if (reason != (eChangeGrowTypeReason)0x1)
            {
                // ORIG：makeRequest 两分支独立，每分支各自重调 getCurCharacNo/get_acc_id
                // （访问器结果禁缓存，战术 P183/P48）。
                if (secondGrew == 0)
                {
                    DB_InsertArchieveEventLog::makeRequest(
                        get_acc_id(), getCurCharacNo(),
                        (SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE)10u);
                }
                else
                {
                    DB_InsertArchieveEventLog::makeRequest(
                        get_acc_id(), getCurCharacNo(),
                        (SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE)0xb);
                }
            }
        }
    }

    // [A06 2026-09-10 P226 扩展 试行记录·已回退] ORIG 0x086640c5 段 env/now 无槽 + 短路岛
    // `mov $1,%eax; jmp; mov $0,%eax; test %al,%al; je` ⇒ 试语句表达式形实测**反劣**
    // （205→210 条：4.4 对语句表达式岛在多条件链上产两处岛/多 5 条）⇒ 回退。
    bool gmTimeActive = false;
    {
        CEnvironment* env = G_CEnvironment();
        // CEnvironment+0x378 = m_serverEnvir.m_serverGroup 整数==10 → gm server
        unsigned int now = GlobalData::s_systemTime_.getCurSec();
        if (env->m_serverEnvir.m_serverGroup == 10 && now < 0x478e7051u)
            gmTimeActive = true;
    }
    if (gmTimeActive && reason != (eChangeGrowTypeReason)0x1 && secondGrew == 0)
    {
        char msg[256];
        strncpy(msg, g_scriptStringManager_.findString(4, "game_server_msg_13", 0), 0xff);
        Inven_Item gift;
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            g_scriptStringManager_.findString(4, "game_server_msg_09", 0),
            gift, 150000, getCurCharacNo(), msg, strlen(msg), 0, GetServerGroup(), 0, 0);
    }

    // [A06 2026-09-10 P226 扩展 试行记录·已回退] ORIG 0x0866414d 段同形（无槽短路岛）；
    // 两条语句表达式同批实测净 +5 条（含本段）⇒ 一起回退。
    bool premiumOn =
        (isAffectedPremium((ENUM_PREMIUM_TYPE)0x10) == true) &&
        (GlobalData::s_systemTime_.getCurSec() <= 0x46e1bbd0u);
    if (premiumOn && reason != (eChangeGrowTypeReason)0x1 && secondGrew == 0)
    {
        // ORIG 0x08664185：`call getCurCharacInvenR; … call GetInvenRef(0,0xb);
        // mov 0x2(%eax),%eax; sub $0x65d6,%eax; cmp $0x4,%eax` —— 两 accessor 结果
        // **均无具名槽**（inven/ref 旧形各多一对 store/load，P0 判据⑦）⇒ 内联链式。
        if ((getCurCharacInvenR()->GetInvenRef(0, 0xb)->m_addInfo - 0x65d6u) < 5u)
        {
            char msg[256];
            strncpy(msg, g_scriptStringManager_.findString(4, "game_server_msg_07", 0), 0xff);
            Inven_Item gift;
            gift.m_addInfo = 0x43d;
            ((char*)&gift)[0] = 2;                          // m_type=2（Inven_Item+0x00 byte）【推断字段序，见 fields_A】
            // +0x14 count 写点：ORIG gift count 由 create_item 内部填【报告核对项】
            
            gift.m_addInfo = 0;
            gift.ResetItemAttr();
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                g_scriptStringManager_.findString(4, "game_server_msg_08", 0),
                gift, 0, getCurCharacNo(), msg, strlen(msg), 0, GetServerGroup(), 0, 0);
        }
    }
