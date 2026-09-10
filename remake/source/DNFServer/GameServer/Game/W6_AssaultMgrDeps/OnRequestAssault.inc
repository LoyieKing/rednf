bool CAssaultMgr::OnRequestAssault(CUser* attacker, CUser* target, int* result)
{
    CAssaultPlace* acquiredPlace = 0;                   // -0x94(%ebp)
    unsigned int nextPlaceId = 0;                       // -0x98(%ebp)
    int chargeMoney = 0;                                // -0x9c(%ebp)

    try
    {
        if (attacker->isCompetitionMercenary() != 0)
        {
            throw static_cast<ENUM_ERROR>(0xea);
        }
        if (target->isCompetitionMercenary() != 0)
        {
            throw static_cast<ENUM_ERROR>(0x15);
        }

        if (!PowerWarOn())
        {
            // ORIG 082ee06e/082ee07b：attacker 的 AssaultedUserId vs target 的 unique_id
            // （旧实现两侧都取 target，真语义 bug）。
            if (attacker->GetCurCharacAssaultedUserId()
                != (static_cast<unsigned short>(target->get_unique_id())))
            {
                LogManager::logFormat(
                    1, "Assault.cpp",
                    __PRETTY_FUNCTION__,
                    0x1291,
                    "CAssaultMgr::OnRequestAssault ERROR - pAssaultedUser Unique ID is invalid");
                throw static_cast<ENUM_ERROR>(0x13);
            }
        }

        bool preventPeerFlag = false;
        if (expert_job::CAlchemist::GetAssaultPrevent(attacker, &preventPeerFlag))
        {
            if (!PowerWarOn())
            {
                if (!preventPeerFlag)
                {
                    throw static_cast<ENUM_ERROR>(0xfb);
                }
                throw static_cast<ENUM_ERROR>(0xfd);
            }
            else if (attacker->getCurCharacVill() != 0x7)
            {
                if (!preventPeerFlag)
                {
                    throw static_cast<ENUM_ERROR>(0xfb);
                }
                throw static_cast<ENUM_ERROR>(0xfd);
            }
        }

        if (GlobalData::s_villageMonsterMgr != 0
            && GlobalData::s_villageMonsterMgr->OnEvent())
        {
            throw static_cast<ENUM_ERROR>(0xd9);
        }

        // P0：ORIG 082ee238/082ee331 各自独立 call IsPowerWarEventOn/getCurCharacVill
        // （无槽往返 ⇒ 原码无具名 bool 局部）。
        if (PowerWarOn() && attacker->getCurCharacVill() == 0x7 && result == 0)
        {
            GlobalData::s_power_manager->GetPowerWarLog()->IncAssaultAttemptCount();
        }

        // ORIG 082ee298 直接解引用返回值（无判空、无 event 具名槽，P171/P0）。
        if (GlobalData::s_event_manager->GetRepeatEvent(0x2a)->IsEventing(0))
        {
            throw static_cast<ENUM_ERROR>(0xb2);
        }

        if (attacker->getCurCharacVill() == 0x8)
        {
            throw static_cast<ENUM_ERROR>(0x13);
        }

        if (!PowerWarOn())
        {
            if (attacker->getCurCharacVill() == 0x7)
            {
                throw static_cast<ENUM_ERROR>(0x13);
            }
        }
        else
        {
            if (attacker->getCurCharacVill() != 0x7)
            {
                throw static_cast<ENUM_ERROR>(0xfc);
            }
            // ORIG 082ee349/082ee35e：实参内联（area 留 %ebx 无槽，P0/P183）。
            if (G_GameWorld()->IsPowerWarSafeZone(
                    static_cast<int>(attacker->getCurCharacVill()),
                    attacker->get_area(false)))
            {
                throw static_cast<ENUM_ERROR>(0x13);
            }
            if (attacker->getPowerSide() == target->getPowerSide()
                || attacker->isJoinPowerWar() == 0
                || target->isJoinPowerWar() == 0)
            {
                throw static_cast<ENUM_ERROR>(0x13);
            }
            CParty* party = static_cast<CParty*>(attacker->GetParty());
            if (party != 0 && party->CanDoPartyActionAssalutState() == 0)
            {
                throw static_cast<ENUM_ERROR>(0x13);
            }
            if (target->GetCurCharacSuperState() != 0)
            {
                throw static_cast<ENUM_ERROR>(0x13);
            }
        }

        if (expert_job::CAlchemist::GetAssaultPrevent(
                target, static_cast<bool*>(0)))
        {
            if (!PowerWarOn())
            {
                throw static_cast<ENUM_ERROR>(0x11);
            }
        }

        if (G_CEnvironment()->m_serverEnvir.m_serverGroup == 3)
        {
            // ORIG 082ee5b6/082ee5c8 两次独立 getCurCharacNo（无 targetNo 局部）。
            if (target->getCurCharacNo() == 0x1eaa0d
                || target->getCurCharacNo() == 0x2c55a8)
            {
                throw static_cast<ENUM_ERROR>(0x13);
            }
        }

        if (!m_bAssaultChannel)
        {
            const int blackCount = target->getCurrCharacBlackCount();
            if (blackCount < 100 && target->GetCurCharacChaosPoint() == 0)
            {
                throw static_cast<ENUM_ERROR>(0x8);
            }
        }

        if (attacker->GetAssaultPlace() != 0)
        {
            throw static_cast<ENUM_ERROR>(0x13);
        }
        if (target->GetAssaultPlace() != 0)
        {
            throw static_cast<ENUM_ERROR>(0x12);
        }
        if (attacker->getCurCharacR() == 0)
        {
            throw static_cast<ENUM_ERROR>(0x13);
        }
        if (attacker->getCurCharacInvenR() == 0)
        {
            throw static_cast<ENUM_ERROR>(0x13);
        }
        if (attacker->getCurCharacNo() == target->getCurCharacNo())
        {
            throw static_cast<ENUM_ERROR>(0x13);
        }

        // 曼哈顿距离 |dx|+|dy| > 100 → 0x63。
        unsigned int dx = attacker->get_posX() & 0xffff;
        dx -= target->get_posX() & 0xffff;
        int distX = static_cast<int>(dx) >> 31;
        int absDx = static_cast<int>((dx ^ distX) - distX);
        bool tooFar = false;
        if (absDx >= 0x65)
        {
            tooFar = true;
        }
        else
        {
            unsigned int dy = attacker->get_posY() & 0xffff;
            dy -= target->get_posY() & 0xffff;
            int distY = static_cast<int>(dy) >> 31;
            int absDy = static_cast<int>((dy ^ distY) - distY);
            if (absDy > 100)
            {
                tooFar = true;
            }
        }
        if (tooFar)
        {
            throw static_cast<ENUM_ERROR>(99);          // ORIG movl $0x63
        }

        if (attacker->getCurCharacVill() == target->getCurCharacVill())
        {
            if (attacker->get_area(false) == target->get_area(false))
            {
                tooFar = false;
            }
            else
            {
                tooFar = true;
            }
        }
        else
        {
            tooFar = true;
        }
        if (tooFar)
        {
            throw static_cast<ENUM_ERROR>(0x13);
        }

        if (!PowerWarOn())
        {
            if (IsStayGate(attacker) || IsStayGate(target))
            {
                throw static_cast<ENUM_ERROR>(0x13);
            }
        }

        // ORIG 082ee95a/082ee9ba 两次独立 GetInstancePrivateStoreMgr（无 storeMgr 局部）。
        private_store::CPrivateStore* store =
            private_store::GetInstancePrivateStoreMgr()->FindStoreSeller(attacker);
        if (store != 0 && store->isDollCreated() == 0)
        {
            throw static_cast<ENUM_ERROR>(0x3c);
        }
        store = private_store::GetInstancePrivateStoreMgr()->FindStoreSeller(target);
        if (store != 0 && store->isDollCreated() == 0)
        {
            throw static_cast<ENUM_ERROR>(0x3c);
        }

        CParty* attackerParty = 0;
        if (attacker->CheckInParty() != 0)
        {
            attackerParty = static_cast<CParty*>(attacker->GetParty());
        }
        if (attackerParty != 0 && attackerParty->IsExistUser(target) != 0)
        {
            throw static_cast<ENUM_ERROR>(0x13);
        }
        if (PowerWarOn()
            && target->getCurCharacVill() == 0x7)
        {
            CParty* targetParty = static_cast<CParty*>(target->GetParty());
            if (targetParty != 0 && targetParty->IsThereGhostInParty() != 0)
            {
                throw static_cast<ENUM_ERROR>(0x13);
            }
        }

        AvailableAssault(attacker, attacker);
        AvailableAssault(target, attacker);

        chargeMoney = attacker->GetCurCharacAssaultMoney();
        // ORIG 082eeb34/082eeb62 两次 getCurCharacInvenR + get_money（差额在异常路径重算）。
        if (static_cast<const CInventory*>(attacker->getCurCharacInvenR())->get_money()
            < chargeMoney)
        {
            chargeMoney =
                chargeMoney
                - static_cast<const CInventory*>(attacker->getCurCharacInvenR())->get_money();
            throw static_cast<ENUM_ERROR>(0xa);
        }

        // param_3 非空＝纯校验模式（DryRun）：通过全部校验后立即成功返回。
        if (result != 0)
        {
            return true;                                 // ORIG +0xbf3 jne +0x11a9
        }

        CAssaultPlaceFactory* factory = GetInstanceAssaultPlaceFactory();
        acquiredPlace = factory->AcquireAssaultPlace();
        if (acquiredPlace == 0)
        {
            throw static_cast<ENUM_ERROR>(0x16);
        }
        acquiredPlace->Reset();
        nextPlaceId = _GetNextAssaultPlaceId();
        acquiredPlace->SetAssaultPlaceId(nextPlaceId);
        acquiredPlace->MakeTeam(attacker, target, chargeMoney);

        std::pair<std::map<unsigned int, CAssaultPlace*>::iterator, bool> inserted =
            m_places.insert(std::make_pair(nextPlaceId, acquiredPlace));
        (void)inserted;

        const int authen = acquiredPlace->GetAuthenValue();
        const int placeId = acquiredPlace->GetAssaultPlaceId();
        TimerCompleteLoadAssaultTimeout::registNextTimer(0x3c, placeId, authen);

        if (PowerWarOn()
            && attacker->getCurCharacVill() == 0x7)
        {
            chargeMoney = 0;
        }

        if (chargeMoney > 0)
        {
            Inven_Item equipSlot =
                static_cast<const CInventory*>(attacker->getCurCharacInvenR())->GetInvenSlot(0, 0xb);
            const int itemIdx = equipSlot.m_addInfo;
            CDataManager* cdm = G_CDataManager();
            const CItem* discountItem =
                (cdm != 0) ? cdm->find_item(itemIdx) : 0;
            if (discountItem != 0)
            {
                const float discount =
                    static_cast<const CEquipItem*>(discountItem)
                        ->getUpgradeCostDiscount();
                const float pricedF =
                    ((discount * static_cast<float>(chargeMoney)) / (-100.0f)
                     + static_cast<float>(chargeMoney));
                chargeMoney = static_cast<int>(pricedF);    // ORIG fstcw/fistpl 截断
            }
            CInventory* invenW = attacker->getCurCharacInvenW();
            invenW->use_money(chargeMoney, static_cast<eMoneySubReason>(0x15), true);
            attacker->SetCurCharacAssaultMoney(0);
            attacker->SetCurCharacAssaultedUserId(0);
        }
        attacker->IncCurCharacAssaultCount();

        if (PowerWarOn()
            && attacker->getCurCharacVill() == 0x7)
        {
            const int joinCount = acquiredPlace->GetAssaulterCount();
            // ORIG 082eeeb1/082eeecd 两次独立 GetPowerWarLog（无 log 局部）。
            GlobalData::s_power_manager->GetPowerWarLog()->IncTotalJoinAssaultUser(joinCount);
            GlobalData::s_power_manager->GetPowerWarLog()->IncAssaultAttemptSuccessCount();
        }

        _SendVictoryInfo(attacker, target);

        {
            PacketGuard packet;
            packet.put_header(0, 0x8);
            packet.put_short(
                static_cast<int>(
                    static_cast<unsigned short>(target->get_unique_id())));
            packet.put_byte(4);
            packet.put_int(chargeMoney);
            packet.finalize(true);
            attacker->Send(packet);
        }
        return true;
    }
    catch (ENUM_ERROR err)
    {
        if (PowerWarOn()
            && attacker->getCurCharacVill() == 0x7 && result == 0)
        {
            // ORIG catch 内计数用 IncAssaultAttemptFailureCount（0x82f0850）
            GlobalData::s_power_manager->GetPowerWarLog()
                ->IncAssaultAttemptFailureCount();
        }
        if (result != 0)
        {
            *result = static_cast<int>(err);
            return false;
        }
        FreeAssaultPlace(static_cast<int>(nextPlaceId), acquiredPlace);
        {
            PacketGuard packet;
            packet.put_header(1, 0xa);
            packet.put_byte(0);
            packet.put_byte(static_cast<int>(err));
            packet.put_byte(4);                     // ORIG 082ef0ce（旧实现漏写，真语义缺口）
            packet.put_int(chargeMoney);
            packet.finalize(true);
            attacker->Send(packet);
        }
        return true;
    }
