// ORIG processKillMonster(CParty*, MapInfo&, map_monster&, vector<bool>&, int, list&)
// 0x084add20：全局效果 1/4 放大掉落次数（向零截断换算，min/max 夹取）；
// 统计队员 premium 0x4f 数并取加成；hell 组计入全灭标记；最终转发
// CBattle_Field::_MakeDropItems。
// b28d-solo-GmCmds_E：P167 调用点差集 CLEAR（21/21）；282=280（ratio 0.43→
// 0.50 段）。已修：base 类型按 T1/T2 直证改 `unsigned int`（ORIG 0x084add59
// 段 `fistp QWORD [ebp-0x68]` + `mov %eax,-0x2c; mov $0,%edx` 零扩展 +
// `fild QWORD` ⇒ unsigned 语义），加成截断按 T1 写 `(int)(long long)expr`
// （ORIG 第二次 `fistp QWORD` 后取低 32 位）。
// b28d-solo-GmCmds_E-2：282=282（gap 0）——删 `float baseF` 具名局部
// （P0：ORIG getDropRate 返回值直接 fnstcw/fistpll，无 fstps 槽往返）+
// 两处 `if(checkValidUser(i)){body}` 改 `if(!checkValidUser(i)) continue;`
// （P168a：ORIG 0x084adf66 `xor $1;test;jne <循环尾>`）⇒ 全函数 jcc 助记符
// 序列与 ORIG 逐位一致（MNE-DIFF 1→0）。残差为 x87 比较 lowering
// （ORIG `fucompp/fnstsw/test $0x45/sete` vs 4.4 `fucomip/fstp/seta`，N85；
// 需 -march=i486 的 TU 级旗标，路由表禁改）与帧/槽差（N82/N115，ORIG
// push edi/esi/ebx + sub 0x9c vs 0x98；P190 double framePad 实测 0x98→0xa8
// 反劣已回退），语义完成、byte 层封顶。
// [round1-L29 2026-09-10 复核] 判据① CLEAR（38 vs 38 分支，目标无错位）。残差 = ORIG
//   `push edi/esi/ebx + sub $0x9c` vs 我方 `sub $0x98`（callee-saved 3 个差异；P332：源级
//   不可诱导，本函数已无多余具名对象可删）⇒ N（ORIG 0x084add20）。
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
}