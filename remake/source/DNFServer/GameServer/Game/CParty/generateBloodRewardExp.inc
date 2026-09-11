// ============================================================================
// CParty::generateBloodRewardExp —— ORIG 0x85b7ea2：血轮经验生成
// （基础×轮次权重×进出次数权重 → getClearRewardBonusExp 加成聚合）。
// ============================================================================
// b28d-solo-CParty: N 封顶 —— N82 帧 0x140/0x170；349 vs 316
void CParty::generateBloodRewardExp(int slot, int round, unsigned int baseExp,
                                    short inoutCnt)
{
    // b28d-49 判词：N 封顶（N82 帧/槽；P282 虚派发偏移差 52 已核）。P167 CLEAR。@0x85b7ea2
    CUser* user = get_user(slot);                       // -0x1c
    unsigned int exp = 0;                               // -0x20
    // [G7-sub] P0 槽律：ORIG 0x085b7ef3/0x085b7f72 两分支各自 `imul` 后写同一
    // 槽对 -0xd8/-0xd4（0x085b7f46/0x085b7fea 的 fistpll 亦落该槽）⇒ 原码 `raw`
    // 是 if 外的单一具名 long long、两分支内赋值；我方原在两分支内各声明一份
    // （-0x28/-0x24 与 -0x18/-0x14 两组槽 = 16B）⇒ 声明提到 if 外，赋值留分支内。
    long long raw;
    // ORIG generateBloodRewardExp.md 085b7edc `movzbl 0x89f(%eax)` =
    // CDungeon::m_bloodDungeonType（unsigned char 宽度修正：原 *(char*) movsbl）
    if (m_battleField.m_pDungeon->m_bloodDungeonType == 2)             // 最终轮
    {
        // ORIG 0x85b7ea2：round*baseExp 32 位无符号乘法（零扩展 64 位）→
        // fraw 先转 float（fildll/fstps），weight 后取（flds），
        // weight*fraw*10.0f 栈上相乘 → fistpll 截断（无 +0.5）。
        // [G7-sub] ORIG 0x085b7f05 `fstps -0xf4(%ebp)`：本分支内的具名 float。
        raw = (long long)(round * baseExp);
        float fraw = (float)raw;
        exp = (unsigned int)(long long)(
            G_CDataManager()->m_bloodDungeon.m_rate[round] * fraw * 10.0f);
    }
    else
    {
        // [G7-sub] ORIG 0x085b7f84 `fstps -0xf0(%ebp)`：另一槽 ⇒ 本分支内
        // 独立具名 float（与真分支不共用），保持分支内声明。
        raw = (long long)(round * baseExp);
        float fraw = (float)raw;
        // [负结论·2026-09-11 本轮实测] 全函数仅剩 1 条实质差：本行 ORIG
        // `flds (%eax,%edx,4); flds -0xf0(%ebp); fmulp %st,%st(1)`（085b7f??
        // 两操作数各 flds 后 fmulp）vs 我方 `flds …; fmuls -0x10(%ebp)`。
        // 试形：显式 double 提升（`(double)m_rate[round] * fraw`）实测 codegen
        // 零变化（仍 fmuls，赋回 float 时提升被折掉）⇒ 4.4 x87 选择器差异，判 N。
        // `float w1 = fraw * m_rate[round]` 为等价值，保持不动。
        float w1 = G_CDataManager()->m_bloodDungeon.m_rate[round] * fraw;
        exp = (unsigned int)(long long)(w1 *
            G_CDataManager()->m_bloodDungeon.m_vec334[(unsigned int)inoutCnt]);
    }
    if (1000000 < exp)
    {
        LogManager::logFormat(
            1, "party.cpp",
            __PRETTY_FUNCTION__,
            0x3cec,
            "BLOOD_LOG : EROOR BLOOD CLEAR REWARD EXP(%u), BASE EXP (%u), "
            "EXP_WEIGHT(%f), EXP_INOUT_WEIGHT(%f), LAST_ROUND(%d), "
            "INOUT_CNT(%d)\n",
            exp, baseExp,
            G_CDataManager()->m_bloodDungeon.m_rate[round],
            G_CDataManager()->m_bloodDungeon.m_vec334[(unsigned int)inoutCnt],
            round, (int)inoutCnt);
    }

    stClearRewardBonusExp_t bonus;                      // -0x68
    stClearRewardExpParameter_t param;                  // -0xc8
    param.m_avatarBonus = getAvatarBonus();
    param.m_creatureBonus = getCreatureBonus(user);

    // [G7-sub] P0 槽律：ORIG 0x085b80f5/0x085b8116 两次 GetRepeatEvent(8) 均**无槽往返**
    // （结果直进 %eax → `mov (%eax),%edx; add $0x34,%edx; mov (%edx),%ecx; call *%ecx`）
    // ⇒ 原码单表达式链，无 ev8/pcr 具名指针；我方原两槽（-0x54/-0x50）已删。
    if (static_cast<CPCRoomBurningEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(8))->IsEventing(user))
        // [负结论·本轮实测] 本处 ORIG 0x085b8130 为**整数除**（`mov $0x51eb851f,%edx;
        // imul; sar $0x5; sar $0x1f; sub` 10 条 + `fildl -0xd0; fstps -0xb4`）⇒
        // `/ 100` 正确；改 `/100.0f` 实测 323→331 反劣（-8 条 ORIG 独有段消失），已回滚。
        param.m_pcRoomBurningRate = (float)(static_cast<CPCRoomBurningEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(8))->GetExpFactor() / 100);

    // [G7-sub] P0 槽律：ORIG 0x085b8169 GetRepeatEvent(3) 亦无槽（同形虚调用
    // `mov (%eax),%edx; add $0x34,%edx; mov (%edx),%edx; call *%edx`）
    // ⇒ 删 ev3/expDouble 两具名指针（我方原 -0x4c/-0x48 两槽）。
    if (static_cast<CExpDoubleEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(3))->IsEventing(0))
        param.m_expDoubleRate = (float)(static_cast<CExpDoubleEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(3))->GetExpFactor() / 100.0f);

    CBurningFatigueEvent* fatigue =
        static_cast<CBurningFatigueEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(0xf));
    if (fatigue->IsEventing(0))
        param.m_burningFatigueBonusExp = fatigue->getBonusExpRate(
            reinterpret_cast<CUserCharacInfo*>(user)->getCurCharacUsedFatigue());

    channel_script_t* chScript =
        (channel_script_t*)G_CDataManager()->GetChannelScript();
    // [G7-sub] P0 槽律：ORIG 0x085b8226 get_index 结果存 %esi、0x085b8237
    // get_channel_no 存 %ebx（**均无栈槽**），0x085b8248 第三次独立 call
    // G_CEnvironment() ⇒ 原码三实参为表达式、无 dungIdx/channelNo/env 具名量
    // （我方原 -0x3c/-0x38/-0x34 三槽）⇒ 内联回实参位，靠 4.4 右→左实参求值序
    // 复现 `mov %esi,0xc(%esp); mov %ebx,0x8(%esp); mov %eax,0x4(%esp)`。
    // ORIG generateBloodRewardExp.md 085b824d `mov 0x378(%eax),%eax`（int dword
    // 读）+ Ghidra `(uchar)*(int*)(iVar4+0x378)` 截断。CEnvironment+0x378 =
    // m_serverEnvir(+0x198)+0x1e0 → Server_Envir::m_serverGroup（int，
    // CEnvironment.h 权威布局）。原 movzbl 单字节读为宽度异形，修正为
    // int 读后截断，与 ORIG 指令序列一致。
    // 同律：channelType 亦无槽（0x085b8253 `movzbl %al,%eax`）⇒ 直接强转作实参。
    param.m_serverChannelRate = chScript->getBonusRate(
        static_cast<unsigned char>(G_CEnvironment()->m_serverEnvir.m_serverGroup),
        static_cast<unsigned int>(G_CEnvironment()->get_channel_no()),
        static_cast<unsigned int>(m_battleField.m_pDungeon->get_index()));

    // [语义轴·真 bug] ORIG 0x085b829b / 0x085b82ba 段同为 `fildl -0xd0(%ebp);
    // flds 100.0f; fdivrp` = **浮点除**（本函数三处除法全为浮点）；旧码 `/ 100`
    // 整数除 ⇒ 费率被截断为整数。改 100.0f。
    if (user->IsHavePremiumAdvantage())
        param.m_premiumAdvantageRate = (float)(
            user->GetPremiumInfo()->GetAdvantageExpRate() / 100.0f);

    param.m_serverEventExpRate = (float)(CServerEvent::GetExpRate() / 100.0f);
    param.m_manageBonus = CalcurateManageBonus(
        ManageBonusType::T_0, user, (unsigned int)exp);

    unsigned int bonusExp = getClearRewardBonusExp(user, exp, param, bonus);
    if (1000000 < bonusExp)
    {
        LogManager::logFormat(
            1, "party.cpp",
            __PRETTY_FUNCTION__,
            0x3d12, "BLOOD_LOG : EROOR BLOOD CLEAR REWARD EXP!! (%u)\n",
            bonusExp);
        LogManager::logFormat(
            1, "party.cpp",
            __PRETTY_FUNCTION__,
            0x3d1c,
            "BLOOD_LOG : EXP PARAMETER : %f, %f, %f, %f, %f, %f, %f, %d\n",
            param.m_avatarBonus, param.m_creatureBonus,
            param.m_pcRoomBurningRate, param.m_expDoubleRate,
            param.m_serverChannelRate, param.m_premiumAdvantageRate,
            param.m_serverEventExpRate, param.m_burningFatigueBonusExp);
    }

    // P183：ORIG 该点直接 `add $0xb24` 取址（无引用缓存槽）。
    reinterpret_cast<CBattle_Field*>(&m_battleField)
        ->m_bloodReward.setRewardExp(slot, bonusExp);
}