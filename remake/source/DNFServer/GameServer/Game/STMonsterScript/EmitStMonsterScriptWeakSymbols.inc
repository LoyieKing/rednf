void EmitStMonsterScriptWeakSymbols()
{
    // 用户内联实现：C1/C2 + clear
    STAtkSuper atkSuper;          atkSuper.clear();
    STCoolTime coolTime;          coolTime.clear();
    STDifficulty difficulty;      difficulty.clear();
    STEventSuper eventSuper;      eventSuper.clear();
    STJumpAttack jumpAttack;      jumpAttack.clear();
    STSummonInfo summonInfo;      summonInfo.clear();
    STThrowAttack throwAttack;    throwAttack.clear();
    STAttackMethod attackMethod;  attackMethod.clear();
    STCounterAttack counterAttack; counterAttack.clear();
    STDash dash;                  dash.clear();
    STHold hold;                  hold.clear();
    STWarp warp;                  warp.clear();
    STSummon summon;              summon.clear();
    STMosterAttackReady attackReady; attackReady.clear();

    // 隐式拷贝构造/拷贝赋值（P74：编译器按成员序合成）
    STCoolTime ctCopy(coolTime);  coolTime = ctCopy;
    STThrowAttack taCopy(throwAttack);  throwAttack = taCopy;
    STInitialTarget initTarget;   STInitialTarget itCopy(initTarget);
                                initTarget = itCopy;
    STRidableSkillExplain skillExplain;
    STRidableSkillExplain seCopy(skillExplain);  skillExplain = seCopy;
    STMosterAttackReady arCopy(attackReady);  attackReady = arCopy;
    STRidableSkill rSkill;        STRidableSkill rsCopy(rSkill);
                                rSkill = rsCopy;

    // 用户 ctor（非 clear 类）
    STMonsterParameterCategory category;

    // STAIInfoScript：destroy W 0x0836753a（头内联实现）+ 隐式拷贝
    // 构造/赋值（ORIG C1ERKS_/aSERKS_ 均为编译器合成版，P74）。
    STAIInfoScript aiInfo;
    aiInfo.destroy();
    STAIInfoScript aiCopy(aiInfo);   // → C1ERKS_/C2ERKS_
    aiInfo = aiCopy;                 // → aSERKS_

    // STMonsterScript：隐式 op=（ORIG W 0x08377994 编译器合成版）；
    // ORIG 无 C1ERKS_，故只 force-use 赋值不触发拷贝构造。
    STMonsterScript monster1;
    STMonsterScript monster2;
    monster1 = monster2;

    // 无用户 ctor 类的隐式 C1（STRidableSkill/STInitialTarget/
    // STRidableSkillExplain 已在上面默认构造触发）

    // 隐式 op=（cpp 已定义 C1/clear）
    STMonsterEquipment eq1;       STMonsterEquipment eq2;
    eq1 = eq2;

    // POD 类显式析构（D1/D2；trivial 析构容器路径不实例化）
    CatchItemInfo catchItem;
    catchItem.~CatchItemInfo();
    stEnamyDropAvatarElement_t dropElement;
    dropElement.~stEnamyDropAvatarElement_t();

    (void)atkSuper; (void)coolTime; (void)difficulty; (void)eventSuper;
    (void)jumpAttack; (void)summonInfo; (void)throwAttack; (void)attackMethod;
    (void)counterAttack; (void)dash; (void)hold; (void)warp; (void)summon;
    (void)attackReady; (void)ctCopy; (void)taCopy; (void)initTarget;
    (void)itCopy; (void)skillExplain; (void)seCopy; (void)arCopy;
    (void)rSkill; (void)rsCopy;
    (void)aiInfo; (void)aiCopy; (void)monster1; (void)monster2;
    (void)category;
}