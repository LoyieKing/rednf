// [J10 2026-09-11 复核] orig 813 / our 818（gap −5）；p167 107/107 CLEAR（CPacketHandler
// C1↔C2/D1↔D2 同址别名 + Inven_Item 别名）。差异 = 槽号 −4 平移：ORIG 0x0846318f
// `mov -0x28(%ebp),%eax; imul $0x3d,%eax,%eax; add -0x24(%ebp),%eax` ↔ 我方
// `add -0x28(%ebp),%eax`（同一 0x3d 步长二维访问，仅槽号差）+ 帧 0x14c vs 0x15c
// ⇒ N82/N95/N115 族，源形不可择（msg 线性 cmp/jne 链与 6 路分发语义已全等）。
int CDeathTower::onTimer(TIMER_MESSAGE msg, unsigned int key)
{
    // [b28d-solo-CDeathTower] verdict=DIFF（orig 806 / our 787，P167 CLEAR 107/107）：
    // 语义（6 路 msg 分发 + 各定时器处理）与调用点全等；残差 = 帧/槽号整体平移
    // 与实参跨 call 落槽（N82/N95/N7 族）⇒ byte 层 N 封顶。
    if (!_checkTimerKey(msg, key))
    {
        return 0;
    }

    if (getTowerState() == 0)
    {
        return 0;
    }

    // [b28d-solo-CDeathTower] ORIG 0x08462b02..0x08463630：线性 cmp/jne 链
    //（cmpl $0x49/0x4b/0x4c/0x4d/0x4e/0x4a,0xc(%ebp)）⇒ 源形是 if-else 链
    //（P17/P251：switch 在 4.4 生跳表+范围检查）。顺序照 ORIG：49,4b,4c,4d,4e,4a。
    if (msg == (TIMER_MESSAGE)0x49)
    {
        int memberCnt = m_party->get_member_count();
        if (memberCnt == 0)
        {
            _destroy();
            return 1;
        }

        unsigned char someoneLeft = 0;
        for (int i = 0; i < 4; ++i)
        {
            CUser* user = 0;
            user = m_party->get_user(i);
            if (!(user != 0 && m_party->checkValidUser(i)))
                continue;
            if (!m_playData.isMemberReady(i))
            {
                // ORIG 0x08462bca：makeFinishLoading 包后单发该用户再请离
                CPacketHandler packet;
                packet.makeFinishLoading();
                user->Send(packet);
                handleLeaveUser(user);
                someoneLeft = 1;
            }
        }

        memberCnt = m_party->get_member_count();
        // [b28d-SCT] P59 块布局判据（ORIG 0x08462c43..0x08462d01）：
        // `cmpb $0x0,-0x59; je 0x08462ce7` + `cmpl $0x0,-0x58; jle 0x08462ce7`
        // 两条独立短路跳向同一 else 块（0x08462ce7 才是 _destroy），fall-through
        // 即 ready+发包体 ⇒ 原码为**正向**条件 `someoneLeft && memberCnt > 0`；
        // 旧反向写法 `!someoneLeft || memberCnt < 1` 令 4.4 把 _destroy 块排前，
        // call 序错位（_destroy@15 vs ORIG@22）。语义等价（De Morgan）。
        if (someoneLeft && memberCnt > 0)
        {
            for (int i = 0; i < 4; ++i)
            {
                m_playData.setMemberReady(i, true);
            }
            // ORIG 0x08462ca0：全员置 ready 后广播 makeFinishLoading
            CPacketHandler packet;
            packet.makeFinishLoading();
            m_party->send_to_party(packet);
        }
        else
        {
            if (memberCnt == 0)
            {
                _destroy();
            }
        }
    }
    else if (msg == (TIMER_MESSAGE)0x4b)
    {
        if (getTowerState() < 3)
        {
            return 0;
        }
        for (int i = 0; i < 4; ++i)
        {
            CUser* user = 0;
            user = m_party->get_user(i);
            if (!(user != 0 && m_party->checkValidUser(i)))
                continue;
            CPacketHandler packet;
            // ORIG 0x08462d97..0x08462f2a：实参内联求值（R→L：item3..item0, exp），
            // 值经 callee-saved/实参槽直传，无具名局部（P0/P52）。
            packet.makeDisplayReward(
                m_playData.getRewardExp(i),
                m_playData.getRewardItemCount(0), m_playData.getRewardItem(0),
                m_playData.getRewardItemCount(1), m_playData.getRewardItem(1),
                m_playData.getRewardItemCount(2), m_playData.getRewardItem(2),
                m_playData.getRewardItemCount(3), m_playData.getRewardItem(3));
            deathTowerCardStatistic(m_playData.getRewardItemCount(i), user,
                                    m_playData.getRewardItem(i));
            user->Send(packet);
        }
        setTowerState((ENUM_TOWER_STATE)4);
        int tkey = _genTimerKey((TIMER_MESSAGE)0x4c);
        G_TimerQueue()->InsertTimer(
            (TimerEntry::OBJ_TYPE)6, m_idx, (TIMER_MESSAGE)0x4c, 5, tkey, 0x4c);
    }
    else if (msg == (TIMER_MESSAGE)0x4c)
    {
        if (getTowerState() < 4)
        {
            return 0;
        }
        bool allHaveItem = true;
        for (int i = 0; i < 4; ++i)
        {
            CUser* user = 0;
            user = m_party->get_user(i);
            if (!(user != 0 && m_party->checkValidUser(i)))
                continue;
            Inven_Item item;
            const CInventory* inven = (const CInventory*)user->getCurCharacInvenR();
            int invenResult = inven->GetInvenData(0x1057, item);
            if (invenResult < 0)
            {
                allHaveItem = false;
                break;
            }
        }

        // ORIG 0x084630da：makeDisplayEPLP(allHaveItem) 后广播
        CPacketHandler packet;
        packet.makeDisplayEPLP(allHaveItem);
        m_party->send_to_party(packet);

        for (int i = 0; i < 4; ++i)
        {
            CUser* user = 0;
            user = m_party->get_user(i);
            if (!(user != 0 && m_party->checkValidUser(i)))
                continue;
            for (int j = 0; j < m_playData.getRewardItemCount(i); ++j)
            {
                Inven_Item* rewardBase = m_playData.getRewardItem(i);
                    // ORIG：rewardBase 为成员奖励数组，逐 j 项发放（m_addInfo +0x02）
                    if (rewardBase[j].m_addInfo == -1)
                    {
                        continue;
                    }
                    int slot = -1;
                    if (rewardBase[j].m_addInfo == 0)
                    {
                        // 金钱奖励：金额 = rewardBase[j].get_add_info()（ORIG 三次调用）
                        // [b28d-SCT] P52/P0：ORIG call 序 get_add_info(60) →
                        // getCurCharacInvenW(61) → gain_money(62) ⇒ 链式单表达式，
                        // 实参先物化、receiver 后取；旧具名 inven 局部令顺序反转。
                        int gained = ((CInventory*)user->getCurCharacInvenW())->gain_money(
                            rewardBase[j].get_add_info(), (eMoneyAddReason)5, true, 0);
                        if (gained < rewardBase[j].get_add_info())
                        {
                            user->SendMoneyFullReason(
                                (ENUM_MONEY_FULL_REASON)0,
                                rewardBase[j].get_add_info(), gained);
                        }
                        slot = 0;
                        // [b28d-SCT] P0：ORIG 0x0846329f 段 `call getDungeon; call
                        // get_index; cmp $0x2af8,%eax; sete/test; je` —— 无 dungeon/
                        // dungeonIdx 具名槽（结果 %eax 直传比较）；旧两局部多 2 槽。
                        if (m_dungeonMgr.getDungeon()->get_index() == 11000)
                        {
                            GetInstanceCirculationStatistic()->AddCirculationStatistic((CIRCULATION_STATISTIC_FIELD)4, user, (unsigned int)gained);
                        }
                        else
                        {
                            GetInstanceCirculationStatistic()->AddCirculationStatistic((CIRCULATION_STATISTIC_FIELD)5, user, (unsigned int)gained);
                        }
                    }
                    else
                    {
                        CInventory* inven = (CInventory*)user->getCurCharacInvenW();
                        slot = inven->insertItemIntoInventory(rewardBase[j], (eItemAddReason)0xb, true, true);
                    }

                    if (slot >= 0)
                    {
                        // ORIG 0x08463394：GetItemSpace + makeUpdateItemList 单发
                        // [b28d-SCT] P0/P23：ORIG call 序 C2(74) → GetItemSpace(75) →
                        // makeUpdateItemList(76) ⇒ 先构造 packet，GetItemSpace 在实参位
                        // 求值；旧具名 space 局部令 GetItemSpace 提前到构造之前。
                        CPacketHandler updatePacket;
                        updatePacket.makeUpdateItemList(
                            user, (ENUM_ITEMSPACE)rewardBase[j].GetItemSpace(), slot);
                        user->Send(updatePacket);
                    }
                }

                // ORIG 0x084635b3：calcHelpAbuseRatio(exp, false)；gain_exp_sp 另取
                user->calcHelpAbuseRatio(m_playData.getRewardExp(i), false);
                int a = 0, b = 0;
                if (user->gain_exp_sp(m_playData.getRewardExp(i), a, b,
                                      (CUser::eExpAddReason)2, 0, false))
                {
                    user->SendNotiPacket((CUser::eSendTarget)2, (ENUM_NOTIPACKET)2, 1);
                }
        }

        setTowerState((ENUM_TOWER_STATE)5);
        m_party->SetEPLPState(2);
        int tkey = _genTimerKey((TIMER_MESSAGE)0x4d);
        G_TimerQueue()->InsertTimer(
            (TimerEntry::OBJ_TYPE)6, m_idx, (TIMER_MESSAGE)0x4d, 0x3c, tkey, 0x4d);
    }
    else if (msg == (TIMER_MESSAGE)0x4d)
    {
        if (m_party->GetEPLPState() != 2)
        {
            return 0;
        }
        if (m_party->GetSelectedEPLPCmd() == 3)
        {
            return 0;
        }
        _onFinishDeathTower();
    }
    else if (msg == (TIMER_MESSAGE)0x4e)
    {
        if (m_party->GetEPLPState() != 3)
        {
            return 0;
        }
        _onFinishDeathTower();
    }
    else if (msg == (TIMER_MESSAGE)0x4a)
    {
        if (getTowerState() < 2)
        {
            return 0;
        }
        if (m_party->get_member_count() < 1)
        {
            _destroy();
            return 1;
        }
        if (_checkMemberDie())
        {
            _beginTowerClearProcess(false);
        }
        else
        {
            // [b28d-SCT] P0/P52：ORIG 0x084636e2 段 `call getCurrentStage` 结果存
            // callee-saved %ebx 跨 getEndStage 调用，`cmp %eax,%ebx` 直接比较，
            // 无 curStage/endStage 槽；旧两局部多 2 槽（我方 -0x1c/-0x20）。
            if (m_stage.getCurrentStage() == m_dungeonMgr.getEndStage())
            {
                _beginTowerClearProcess(true);
            }
        }
    }

    return 1;
}