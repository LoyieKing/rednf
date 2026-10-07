// ============================================================================
// ORIG T 0x081f63c0 _ZN19DisPatcher_UseStack12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_UseStack/dispatch_sig.md，1320 insn）
// 读 slot(+0)/itemSpace?/itemId/slotIdx/checksum → state 白名单（5,7,8,3,6,
// 10,0xc）→ checksum 反作弊 0x25b → 封印限制 0xd1 → GetItemPtr/CheckItem(0x2a)
// → 可堆叠使用地点 → 组队/TOD/锦标赛限定（错误包 header(1,0x2f) 系列尾部）
// → use_stackable 落地。
// ============================================================================
// §6.2 巨件核销：P167 208/208 [0缺0多]；gap 1320→1320/1321；残差归因 N82/P112
// （diff 形态计数对称无缺类，详见 docs/asm_translation_patterns.md 负例表）。
// [b28d-29] N82/P183：帧 0x120 vs 0x140 + ORIG 两步 add 寻址 vs 4.4 独立局部槽；语义集合全等  (ORIG 0x081f63c0 insn 1320->1321)
// 无真修条目（batch-20 半程核销语义未回退）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 1374（ORIG 1307 / ours 1321），残差=setcc-fold bool 物化岛/P168a④ bool 槽化/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
// [IG3-semantic] 差一候选our `cmpl $0x1e6c` ↔ ORIG `cmp $0x1e6d`（ORIG 0x081f6c30）逐点核实为**等价，记 N**：
//   本文件 9654 行源码写 `itemId >= 0x1e6d`；ORIG 决策树 `cmp $0x1e6d,%eax; jb 0x81f6c4d(default)`，
//   4.4 对无符号 `>=` 发对偶 `cmpl $0x1e6c; jbe 下一比较`（Ghidra C `uVar1 < 0x1e6d ⇒ goto LAB_081f6c4d` 互证）；
//   v<=0x1e6c 路径经 `cmp $0x28978a`（恒不等）后仍落同一错误包块（0x81f6c4d ↔ 我方 183e5 段），无可见行为差。
//   同函数扫描：我方多出的比较常量仅此一处（非发明防御）；ORIG 多的 0x16/0x18/0x33 各 1 次
//   = 4.1 switch 树「je + 范围判定」重复比较，检查常量集合两侧一致（非缺失校验），故不改源码。
int DisPatcher_UseStack::dispatch_sig(CUser* user, PacketBuf& packet)

{
    short slotNo = 0;                            // ORIG local_6e（get_short(short&)/movswl 实证）
    if (!packet.get_short(slotNo))
        return LineFunc(0x8315,
                        __PRETTY_FUNCTION__,
                        0, 0);
    unsigned char itemSpace = 0;                 // ORIG local_7c[0]
    if (!packet.get_byte(itemSpace))
        return LineFunc(0x8318,
                        __PRETTY_FUNCTION__,
                        0, 0);
    int itemAddInfo = 0;                         // ORIG local_80
    int param2 = 0;                              // ORIG local_84
    if (!packet.get_int(itemAddInfo))
        return LineFunc(0x831d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(param2))
        return LineFunc(0x831e,
                        __PRETTY_FUNCTION__,
                        0, 0);

    bool stateOk =
        (user->get_state() == 5) || (user->get_state() == 7) ||
        (user->get_state() == 8) || (user->get_state() == 3) ||
        (user->get_state() == 6) || (user->get_state() == 10) ||
        (user->get_state() == 12);
    // ---- 状态拒绝包形态：header(1,0x2f)/byte0/byte(code)/byte(space)/
    //      int(itemAddInfo)/int(param2)/finalize(true) ----
#define USESTACK_ERR(pg_, code_)                                               \
    do                                                                         \
    {                                                                          \
        pg_.put_header(1, 0x2f);                                               \
        pg_.put_byte(0);                                                       \
        pg_.put_byte(static_cast<int>(code_));                                 \
        pg_.put_byte(static_cast<int>(itemSpace));                             \
        pg_.put_int(static_cast<int>(itemAddInfo));                            \
        pg_.put_int(static_cast<int>(param2));                                 \
        pg_.finalize(true);                                                    \
        user->Send(pg_);                                                       \
    } while (0)

    if (!stateOk)
    {
        PacketGuard pgState;
        USESTACK_ERR(pgState, 0x13);
        return 0;
    }
    int invenCheckSum = 0;                       // ORIG local_88
    if (!packet.get_int(invenCheckSum))
        return LineFunc(0x8338,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if ((unsigned int)user->getCurCharacInvenCheckSum() != invenCheckSum)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x25b, 1, 0, 0);
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, static_cast<RestrictCategory::Enum>(1), 0xc))
    {
        PacketGuard pgRest;
        USESTACK_ERR(pgRest, 0xd1);
        return 0;
    }
    CItem* pItem = GlobalData::s_pSecuProtectionField->GetItemPtr(
        user, static_cast<char>(itemSpace), slotNo);
    if (pItem == 0)
    {
        return LineFunc(0x8354,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int secuErr = GlobalData::s_pSecuProtectionField->CheckItem(
        user, static_cast<SECURITY_PROTCTION>(0x2a), pItem);
    if (secuErr != 0)
    {
        PacketGuard pgSecu;
        USESTACK_ERR(pgSecu, secuErr);
        return 0;
    }
    if (pItem->is_stackable() &&
        !static_cast<CStackableItem*>(pItem)->verifyUsablePlace(user))
    {
        PacketGuard pgPlace;
        USESTACK_ERR(pgPlace, 0x13);
        return 0;
    }

    if (user->GetAssaultPlace() == 0 &&
        (user->get_state() == 3 || user->get_state() == 6))
    {
        // ORIG 4 处均调 const 重载 _ZNK10CInventory11GetInvenRefEii：
        // getCurCharacInvenR() 返 const CInventory*，const 命中重载。
        const CInventory* invR = reinterpret_cast<const CUserCharacInfo*>(user)
            ->getCurCharacInvenR();
        const Inven_Item* ref =
            invR->GetInvenRef(GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)itemSpace),
                              (int)slotNo);
        if (ref == 0)
        {
            return LineFunc(0x83b7,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        unsigned int itemId =
            static_cast<unsigned int>(ref->m_addInfo);   // ORIG ref+2
        CDataManager* dm = G_CDataManager();
        CItem* tmpl = dm->find_item(static_cast<int>(itemId));
        if (tmpl == 0)
        {
            return 0;
        }
        int itemType = tmpl->GetItemType();              // ORIG vtable +0xc
        bool usableFamily =
            (itemType == 3) || (itemType == 0x16) ||
            (itemType == 0x18) || (itemType == 0x23);
        if (!usableFamily)
        {
            usableFamily =
                (itemId == 10) || (itemId == 0x18) ||
                (itemId == 0x33) ||
                (itemId >= 0x1e6d && itemId <= 0x1e73) ||
                itemId == 0x28978a;
        }
        if (!usableFamily)
        {
            PacketGuard pgType;
            USESTACK_ERR(pgType, 0x13);
            return 0;
        }
        // ---- 需要消耗疲劳的特殊物品表（LAB_081f6df8 上方比较树）----
        unsigned short fatigueNeed = 0;
        bool needFatigue = false;
        switch (itemId)
        {
        case 0x1f8f:
        case 0x27ac5a:
        case 0x27addd:
        case 0x292090de:
            fatigueNeed = 0x1e;
            needFatigue = true;
            break;
        case 0x1d5e:
            fatigueNeed = 10;
            needFatigue = true;
            break;
        case 0x1d20:
            fatigueNeed = 0x28;
            needFatigue = true;
            break;
        case 0x1ddc:
        case 0x1ddd:
            fatigueNeed = 6;
            needFatigue = true;
            break;
        case 0x201d:
            fatigueNeed = 0x14;
            needFatigue = true;
            break;
        default:
            break;
        }
        if (needFatigue)
        {
            if (user->get_state() != 3)
            {
                PacketGuard pgFat1;
                USESTACK_ERR(pgFat1, 0x13);
                return 0;
            }
            if (reinterpret_cast<CUserCharacInfo*>(user)
                    ->getCurCharacFatigue() < fatigueNeed)
            {
                PacketGuard pgFat2;
                USESTACK_ERR(pgFat2, 0x5f);
                return 0;
            }
        }
    }

    if (user->get_state() == 10)
    {
        WongWork::CDeathTower* tower = user->getDeathTower();
        if (tower == 0)
        {
            return LineFunc(0x8456,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!tower->handleUseStackable(
                user, static_cast<ENUM_ITEMSPACE>(itemSpace), slotNo))
        {
            return LineFunc(0x8458,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }

    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0)
    {
        WongWork::CDeathTower* tower = 0;
        if (user->get_state() == 10)
            tower = user->getDeathTower();
        if (tower == 0)
        {
            if (party->m_battleField.m_bloodState == 1)
            {
                PacketGuard pgParty1;
                USESTACK_ERR(pgParty1, 0);
                return 0;
            }
            if (party->m_battleField.m_pDungeon != 0 &&
                party->m_battleField.m_pDungeon->isTowerOfDespairDungeon() &&
                party->m_battleField.m_pDungeon->limitOfStackableItemInTower())
            {
                const CInventory* invR2 = reinterpret_cast<const CUserCharacInfo*>(user)
                    ->getCurCharacInvenR();
                const Inven_Item* ref2 =
                    invR2->GetInvenRef(
                        GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)itemSpace),
                        (int)slotNo);
                if (ref2 == 0)
                {
                    return 0;
                }
                unsigned int todItemId =
                    static_cast<unsigned int>(ref2->m_addInfo);
                if (!(todItemId == 0x18 || todItemId == 10 ||
                      todItemId == 0x33 || todItemId == 0x28978a))
                {
                    int dungeonIndex =
                        reinterpret_cast<CBattle_Field*>(
                            &party->m_battleField)
                            ->get_dungeon_index();
                    cMyTrace trace(
                        __PRETTY_FUNCTION__,
                        0x84a2, 5);
                    trace("TOD : charac_no(%u),dungeonIndex(%d),item_id(%lu)",
                          reinterpret_cast<CUserCharacInfo*>(user)
                              ->getCurCharacNo(),
                          dungeonIndex, todItemId);
                    return 0;
                }
            }
            if (party->m_battleField.m_pDungeon != 0 &&
                party->m_battleField.m_pDungeon->isTournamentDungeon())
            {
                const CInventory* invR3 = reinterpret_cast<const CUserCharacInfo*>(user)
                    ->getCurCharacInvenR();
                const Inven_Item* ref3 =
                    invR3->GetInvenRef(
                        GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)itemSpace),
                        (int)slotNo);
                if (ref3 == 0)
                {
                    return LineFunc(0x84b1,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                CItem* tItem =
                    G_CDataManager()->find_item(ref3->m_addInfo);
                if (tItem == 0)
                {
                    return 0;
                }
                if (!tItem->is_stackable())
                {
                    return 0;
                }
                CStackableItem* stack = static_cast<CStackableItem*>(tItem);
                int denyCode = 0;
                if (stack->get_sub_type() == 2)
                {
                    denyCode = 0xff;
                }
                else if (
                     /* ORIG 0x081f75ca `movzbl 0x364(%eax),%eax; test %al,%al`：
                        1B 零扩展读具名字段 m_char364（CStackableItem.h:476，声明为 char，
                        故按 ORIG 无符号宽度取值；原转写作 *(int*)(stack+0xd90) —— 偏移与
                        宽度均与 ORIG 不符）。
                        [DPC_DispA 2026-09-11] 旧形 `*reinterpret_cast<const unsigned char*>
                        (&stack->m_char364)` 取址 ⇒ 4.4 产两步形
                        `mov -0x2c(%ebp),%eax; add $0x364,%eax; movzbl (%eax),%eax`
                        （0x18770-0x18775，disp2 OURS-only 无 `+0x364`、ORIG-only `+0x364`×1）；
                        改 `static_cast<unsigned char>(成员)` 直读 ⇒ 单步 `movzbl 0x364(%eax)`。*/
                     static_cast<unsigned char>(stack->m_char364) != 0 &&
                         stack->GetItemType() != 0x12)
                {
                    denyCode = 0xff;
                }
                if (denyCode != 0)
                {
                    PacketGuard pgTour;
                    USESTACK_ERR(pgTour, denyCode);
                    return 0;
                }
            }
        }
        else
        {
            CDungeon* dg = tower->getCDungeonMGr()->getDungeon();
            // [DNF-FIX-AUDIT 2026-10-07] ORIG 0x081f7157..0x081f717f：`call limitOfStackableItemInTower;
            //   xor $0x1,%eax; test %al,%al; je F` + `mov 0xcd8(party)(=m_battleField.m_bloodState);
            //   cmp $0x1; jne F` ⇒ 拒绝条件 = !limit && bloodState==1（同 tower==0 分支 bloodState==1 拒绝）。
            //   旧形两项极性皆反（limit && bloodState!=1）⇒ 绝望之塔限堆叠层非血战态误拒、血战态误放。
            if (!dg->limitOfStackableItemInTower() &&
                party->m_battleField.m_bloodState == 1)
            {
                PacketGuard pgTower;
                USESTACK_ERR(pgTower, 0);
                return 0;
            }
        }
    }

    {
        const CInventory* invRFin = reinterpret_cast<const CUserCharacInfo*>(user)
            ->getCurCharacInvenR();
        const Inven_Item* refFin =
            invRFin->GetInvenRef(GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)itemSpace),
                                 (int)slotNo);
        if (refFin == 0)
        {
            return 0;
        }
        if (refFin->m_addInfo == 0x27ac4a)
        {
            int lastKillTime =
                user->GetCurCharacChaosKillTime();
            CParty* finParty = static_cast<CParty*>(user->GetParty());
            char chaosErr = 0;                       // ORIG local_d
            int curSec = GlobalData::s_systemTime_.getCurSec();
            if (curSec < lastKillTime + 600)
            {
                chaosErr = -6;
            }
            else if (finParty != 0 &&
                     finParty->CheckMemberLastTryAssaultTime())
            {
                chaosErr = -3;
            }
            if (chaosErr != 0)
            {
                PacketGuard pgChaos;
                USESTACK_ERR(pgChaos, (int)chaosErr);
                return 0;
            }
        }
        if (!user->use_stackable(slotNo, (int)itemSpace,
                                 static_cast<int>(itemAddInfo),
                                 static_cast<int>(param2)))
        {
            return LineFunc(0x8508,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    return 0;
#undef USESTACK_ERR
}
