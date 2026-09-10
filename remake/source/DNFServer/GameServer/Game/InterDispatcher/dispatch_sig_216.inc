int Inter_VillageAttackedEnd::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_VillageAttackedEndPktView* pPkt = reinterpret_cast<Inter_VillageAttackedEndPktView*>(data);
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())   // test al → false 才继续
        return 0;

    const unsigned int curSec =                        // -0x38
        GlobalData::s_systemTime_.getCurSec();
    const unsigned int closeTime =                     // -0x34（msg+0x0a）
        pPkt->m_a;
    const int remainSec =                              // -0x34 复用（Ghidra local_34）
        (closeTime < curSec) ? 0 : (int)(closeTime - curSec);
    int state;                                         // -0x30

    if (pPkt->m_e <
        pPkt->m_12)
    {
        GlobalData::s_revengeDungeonMgr->CloseRevengeDungeon();
        GlobalData::s_revengeDungeonMgr->SetCloseTime(closeTime);
        state = 2;
    }
    else
    {
        GlobalData::s_revengeDungeonMgr->OpenRevengeDungeon();
        GlobalData::s_revengeDungeonMgr->SetCloseTime(closeTime);
        state = 1;
    }

    for (std::map<unsigned short, CUser*>::iterator it =   // GameWorld+0x134
             G_GameWorld()->m_UserInWorld.begin();
         it != G_GameWorld()->m_UserInWorld.end(); it++)   // P28：ORIG 增量段 ppEi ⇒ 后置++
    {
        CUser* target = it->second;                    // ORIG pair+4
        if (target == 0)
            continue;

        char buf[10];                                  // -0x5a
        memset(buf, 0, sizeof(buf));
        PacketGuard packet;                            // -0x68
        packet.clear();
        packet.put_header(0, 0x1ec);
        target->VillageAttack_NotDBUpdate();

        if (state == 1)
        {
            APSystem::CUserProc::ClearActionAndSendtoUser(
                target, (APSystem::_ActionGroupIndex)0x17, 0,
                (ENUM_PACKETCLASS)0);
            unsigned char inDungeon = 0;               // -0x22
            const unsigned char atkCnt =               // -0x21
                target->GetCurVillageAttackCount();
            buf[0] = 0;
            buf[1] = 0;
            *reinterpret_cast<int*>(buf + 2) = remainSec;
            const int st = target->get_state();        // -0x20
            if (st > 2)
            {   // 疲劳扣减（-5，下限 0）并回发
                const int fatigue =
                    std::max(0, (int)target->getCurCharacFatigue() - 5);
                target->setCurCharacFatigue(fatigue);
                target->SendFatigue();
            }
            if (st == 3)
            {
                inDungeon = 1;
                if (atkCnt > 2)
                    *reinterpret_cast<int*>(buf + 6) = 0x1e41;

                typedef std::vector<ConditionEnterRevenge> RevengeCondVec;
                // ORIG 0x84df8e4/905/924/944 四处独立 G_CDataManager + 三次 vector::operator[]
                //（p167 M=G_CDataManager×3 / vector[]×2）⇒ 逐字段重读（P157/P183），禁缓存引用。
