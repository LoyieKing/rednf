// ---- KillMonsterGetExp（ORIG 0x0814d300）----
// 对每名有效队员：get_mob_reward → getMonsterTotalExp → 死亡之塔两层费率 →
// sumGainedEachExp → calcHelpAbuseRatio → gain_exp_sp(+Noti) → CheckQuestMonster。
// [b28d-62] N82：p167 EXTRA getParty/get_party_type 各 1（ORIG 缓存结果）。
//   ORIG 0x0814d300
// [b28d-solo-CBossStage] N82封顶：exp 写回/getMonsterTotalExp + float 截断费率
//   （原 double+0.5 语义错）+ m_party/m_dungeon 直读 + m_expParamAE(P254)。
//   残差 44=4.4 float 浅槽 vs ORIG 深槽（标量分区，N82附）。
bool CBossStage::KillMonsterGetExp(const map_monster& mob, unsigned int& exp)
{
    // P0：ORIG 0x0814d308/0x0814d5da `mov 0x4(%eax),%eax` / `mov 0x9f0(%eax),%eax`
    //   ⇒ m_party/m_dungeon 直访问（无 getParty() 调用、无 party/dungeon 槽）；
    //   0x0814d3bf 起各段逐次重读 mob.m_roleType（无 roleType 具名局部）。
    // ORIG 0x0814d316 返回值落 -0x1c 死槽（P171 照抄，后续无读取）
    // [b28d-solo] ORIG float expF/expF2 在深槽 -0x50/-0x4c、块内 data 在浅槽
    //   -0x24；4.4 恒聚合对象占低槽、标量分区（N82附）。
    // [b28f-SweepCBossStage 三形实测复核] ①本形（expF/expF2 函数头）: AE 差 12 块
    //   （our 226）；②标量全头部声明（sp/sfp/reason 提到头）: our 226→229 条、
    //   槽阶梯全乱（ORIG `movl $0xb,-0x10` 被我方落 -0x1c）⇒ 劣化回滚；
    //   ③expF/expF2 移入 for 体内: AE 差 18 块（our -0x1c/-0x18）⇒ 劣化回滚。
    //   ⇒ 4.4 在本件按「先用者浅」分区，与 ORIG 4.1「先用者深」方向相反，
    //   声明位置不可翻（P212 反向 + N82附），封顶。p167 CLEAR（仅 C1/C2 别名）。
    // [K2ERandomBoss 2026-09-11 负结论] 前置 `char framePad[0x2c]`（P232 撑帧）
    // 试验：帧 sub 0x60→**0x90**（ORIG 0x70，方向相反）、差异块 38/差异行 75
    // **逐值不变** ⇒ 该旋钮不改 float 组槽序，已回滚（ORIG 0x0814d425 fstps
    // -0x50 / 0x0814d484 fstps -0x4c 为 4.1「先用者深」分区，4.4 反向）。
    float expF;
    float expF2;
    int memberLevelGap = m_party->getMemberLevelGap();
    (void)memberLevelGap;

    for (int i = 0; i < 4; ++i)
    {
        CUser* user = 0;             // ORIG 0x0814d325 预清（P198①）
        user = m_party->get_user(i);
        if (user == 0 || !m_party->checkValidUser(i))
            continue;
        if (m_party->m_battleField.m_bloodState == 1)   // PowerWar
            continue;

        G_CDataManager()->get_mob_reward(user->get_charac_level(), exp);
        // ORIG 0x0814d402 返回值直接写回 exp（无 total 具名局部）
        exp = m_party->getMonsterTotalExp(
            exp, mob.m_roleType, m_expParamAE, 1.0f, 1.0f, false);

        // 死亡之塔费率（ORIG 0x0814d422 fildll+fstps / 0x0814d44c flds+fmuls+
        //   fistpl：float 运算 + 截断，无 +0.5 舍入；+0x00=apcExpWeight、
        //   +0xc84=monsterExpWeight）。
        if (mob.m_roleType == 5)
        {
            expF = (float)exp;
            exp = (unsigned int)(int)(G_CDataManager()->GetTowerScript(
                m_dungeon->get_index())->m_apcExpWeight * expF);
        }
        expF2 = (float)exp;
        exp = (unsigned int)(int)(G_CDataManager()->GetTowerScript(
            m_dungeon->get_index())->m_monsterExpWeight * expF2);

        STGainedEachExpData data;                 // ORIG ctor W 0x08151a56
        data.m_exp = exp;
        exp = m_party->sumGainedEachExp(user, data);
        user->calcHelpAbuseRatio(exp, false);

        int sp = 0;
        int sfp = 0;
        int reason = 0xb;            // ORIG 0x0814d532/0x0814d544 if 形非三目
        if (mob.m_roleType == 5)
            reason = 0xc;
        if (user->gain_exp_sp((int)exp, sp, sfp, (CUser::eExpAddReason)reason,
                              mob.m_mobIndex, false))
            user->SendNotiPacket((CUser::eSendTarget)2, (ENUM_NOTIPACKET)2, 1);

        ENUM_QUEST_ENEMY_TYPE questType = (ENUM_QUEST_ENEMY_TYPE)1;
        if (mob.m_roleType > 4 && mob.m_roleType <= 8)
            questType = (ENUM_QUEST_ENEMY_TYPE)2;
        user->CheckQuestMonster(m_dungeon->get_index(), mob.m_mobIndex,
                                questType);
    }
    return true;
}