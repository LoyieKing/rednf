void CMonsterDropTest::processKillMonster(CParty* party, MapInfo& mapInfo,
                                          map_monster& mob, std::vector<bool>& flags,
                                          int killCnt, std::list<map_item>& items)
{
    // ORIG 无 effMgr 具名局部（每次 `mov 0x941f7ec,%eax` 重读全局，P0）。
    if (GlobalData::s_globalEffectManager_->isAffectedEffect(1))
    {
        float effVal = GlobalData::s_globalEffectManager_->getEffectValue(1);
        // ORIG 0x84add20：条件 effVal > (int)baseF（fildll 载入截断整数 base 与
        // effVal 浮点比较）；加成 (base - effVal) * killCnt（符号与 baseF 相反）。
        // T1/T2 直证：ORIG `fistp QWORD [ebp-0x68]` + 回读 `mov %eax,-0x2c;
        // mov %eax,%edx?`→`mov $0,%edx` 零扩展后 `fild QWORD` ⇒ base 为
        // unsigned int（非 int）。
        // P0 直证：ORIG getDropRate 返回值直接 fnstcw/fistpll 转 int64（无
        // fstps 槽往返）⇒ 原码无 baseF 具名 float 局部，须内联。
        unsigned int base =
            static_cast<unsigned int>(G_CDataManager()->getDropRate());
        if (static_cast<float>(base) < effVal)
        {
            // T1 直证：ORIG `fistp QWORD [ebp-0x68]` 后取低 32 位 ⇒ 源码为
            // `(int)(long long)expr`（Ghidra `(int)(longlong)ROUND(..)`）。
            killCnt += static_cast<int>(static_cast<long long>(
                (static_cast<float>(base) - effVal)
                * static_cast<float>(killCnt)));
            int cap = static_cast<int>(static_cast<float>(killCnt) * effVal);
            killCnt = std::max(0, std::min(killCnt, cap));
        }
    }
    if (GlobalData::s_globalEffectManager_->isAffectedEffect(4))
    {
        float effVal = GlobalData::s_globalEffectManager_->getEffectValue(4);
        unsigned int base =
            static_cast<unsigned int>(G_CDataManager()->getDropRate());
        if (static_cast<float>(base) < effVal)
        {
            killCnt += static_cast<int>(static_cast<long long>(
                (static_cast<float>(base) - effVal)
                * static_cast<float>(killCnt)));
            int cap = static_cast<int>(static_cast<float>(killCnt) * effVal);
            killCnt = std::max(0, std::min(killCnt, cap));
        }
    }

    // premium 0x4f 队员计数与其独立掉落加成
    unsigned char premCnt = 0;
    int premRate = 0;
    for (int i = 0; i < 4; i++)
    {
        // ORIG 0x084adf66 `xor $1;test;jne <循环尾>` ⇒ 原码是
        // `if (!checkValidUser(i)) continue;`（P168a：bool callee 裸取反形）。
        if (!party->checkValidUser(i))
        {
            continue;
        }
        CUser* member = party->GetMember()[i].m_pUser;
        if (member->isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(0x4f)))
        {
            premCnt = premCnt + 1;
        }
    }
    if (premCnt != 0 && premCnt < 5)
    {
        for (int i = 0; i < 4; i++)
        {
            if (!party->checkValidUser(i))
            {
                continue;
            }
            CUser* member = party->GetMember()[i].m_pUser;
            premRate = static_cast<int>(
                member->getAddIndependentDropRateFromPremium(
                    static_cast<ENUM_PREMIUM_TYPE>(0x4f), premCnt));
            break;
        }
    }

    bool hellKilledAll = false;
    if (mob.m_hellPartyGroupIdx != 0)
    {
        // P0/P183：ORIG 0x084ae042 `mov 0xc(%ebp),%eax; lea 0xb24(%eax),%edx`
        // 直接作 this 传参，无 `bf` 具名指针槽（我方旧形多 add+store+reload 3 条）。
        party->m_battleField.KillHellPartyGroupMonsterCnt(mapInfo, mob, hellKilledAll);
    }

    // P0：ORIG 0x084ae07b `call get_member_count` 结果留 %eax 直压 0x14(%esp)，
    // 无 memberCount 具名槽（我方旧形多 store/reload 一对）。
    party->m_battleField._MakeDropItems(items, mapInfo, mob,
                                        static_cast<unsigned short>(0),
                                        party->get_member_count(), 100, killCnt,
                                        hellKilledAll, flags, 1.0f, premRate);
