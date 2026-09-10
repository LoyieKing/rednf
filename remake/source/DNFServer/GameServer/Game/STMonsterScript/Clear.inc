void STMonsterScript::Clear()
{
    // 循环变量声明风格经骨架实验校准：i/j 为 decl-init+for-init 双写模式，
    // k/l/m/n 为块作用域单槽（与 ORIG 槽位 -0x20/-0x1c/-0x18/-0x14/-0x10/-0xc 一致）。
    m_flagc50 = false;                       // movb $0,0xc50
    m_flag3a8 = false;                       // movb $0,0x3a8
    m_aiInfo.clear();                        // call STAIInfoScript::clear(this+0x138)
    m_flag33c = true;                        // movb $1,0x33c
    m_flag368 = true;                        // movb $1,0x368
    m_field36c = 0;
    m_field370 = 0;
    m_flag374 = false;
    m_field378 = 0;
    m_field37c = 0;
    m_field380 = 0;
    m_index = 0;                             // movl $0,(%eax)
    m_name = "";                             // call Ss::operator=(PKc)
    m_altName = "";
    m_width = 40;                            // movl $0x28,0xc
    m_height = 20;                           // movl $0x14,0x10
    m_floatingHeight = 0;
    m_layer = 1;
    m_field01c.clear();                      // vector<pair> clear(+0x1c)
    int i = 0;
    for (i = 0; i <= 3; ++i)                 // 首循环：decl+for-init 双写
        m_field028[i].clear();               // vector<pair>[4] 元素 0xc 步进
    m_parameterCategories.clear();           // call(+0x7c)
    m_flag3b8 = true;                        // movb $1,0x3b8
    m_abilityTable = 0;                      // +0x3bc
    m_field3c0 = 0;                          // +0x3c0
    for (i = 0; i <= 9; ++i)
        m_field3c4[i] = 0;                   // l[0x3c4+4i]=0
    for (i = 0; i <= 1; ++i)
    {
        int j = 0;
        for (j = 0; j <= 3; ++j)             // 单一二维表复合寻址（实证）
            m_resistTable[j][i] = 0;         // [0x3f4+8j+4i]=0
        for (j = 0; j <= 16; ++j)
            m_linearAbilities[j][i] = 0;     // [0x414+8j+4i]=0
        m_hpRegenSpeed[i] = 0;               // [0x49c+4i]
        m_mpRegenSpeed[i] = 0;               // [0x4a4+4i]
        m_moveSpeed[i] = 0;                  // [0x4ac+4i]
        m_attackSpeed[i] = 0;                // [0x4b4+4i]
        m_castSpeed[i] = 0;                  // [0x4bc+4i]
        m_hitRecovery[i] = 0;                // [0x4c4+4i]
        m_jumpPower[i] = 0;                  // [0x4cc+4i]
        m_jumpSpeed[i] = 100;                // movl $0x64,[0x4d4+4i]（无分支实证）
        m_normalDashSpeed[i] = 0;            // [0x4dc+4i]
        m_slantDashSpeed[i] = 0;             // [0x4e4+4i]
        m_weight[i] = 0;                     // [0x4ec+4i]
        m_stuckBonusOnDamage[i][0] = 0;      // scale8 +0x4f4
        m_stuckBonusOnDamage[i][1] = 0;      // scale8 +0x4f8
        for (j = 0; j <= 3; ++j)
            m_paramTable[j][i] = 0;          // [0x508+8j+4i]=0
        m_field528[i] = 0;                   // [0x528+4i]
        m_levelByDiff[i] = 0;                // [0x3ec+4i]
    }
    m_sight = 200;                           // movl $0xc8,0x504
    m_comeInAction = "";
    m_comeInActionEx = "";
    m_field538 = 100;
    m_field53c = 200;
    m_field540 = -1.0f;
    m_field544 = -1.0f;
    m_field548 = false;
    m_field54c = 20.0f;
    m_field550 = -300.0f;
    m_field554.clear();
    m_field560.clear();
    m_field56c = false;
    m_targetingTimeTerm = 1000;              // +0x574
    m_targetingOnAttack = false;             // +0x580
    m_targetingOnDamage = false;             // +0x581
    m_targetingLevel = 0;                    // +0x584
    m_targetingHp = 0;                       // +0x588
    m_field58c = 0;                          // +0x58c
    m_targetingNearest = false;              // +0x590
    m_targetingAttacker = false;             // +0x591
    m_targetingFromDirection = false;        // +0x592
    m_targetingLowPhysAttack = 0;            // +0x594
    m_targetingLowPhysDefense = 0;           // +0x598
    m_targetingLowMagAttack = 0;             // +0x59c
    m_targetingLowMagDefense = 0;            // +0x5a0
    m_targetingDowned = false;               // +0x5a4
    m_warlike = 50;                          // movl $0x32,0x578（序在 5a4 后，忠实 ORIG）
    m_attackDelay = 1000;                    // movl $0x3e8,0x57c
    m_superarmorOnAttack = false;            // movb $0,0x5a5
    m_field5a8 = 0;
    m_faceImage = "";                        // assign(+0x5ac)
    m_field5b0 = 0;                          // movl $0,0x5b0
    m_waitingMotion = "";                        // 三连展开（[2..4] 不清，实证）
    m_moveMotion = "";
    m_sitMotion = "";
    for (i = 0; i <= 1; ++i)
        m_damageMotion[i] = "";                  // 循环 ×2 @0x5cc
    m_downMotion = "";                      // 四连展开
    m_overturnMotion = "";
    m_jumpMotion = "";
    m_jumpAttackMotion = "";
    m_attackMotions.clear();
    m_etcMotions.clear();
    m_jumpAttackInfo = "";                 // assign(+0x6b4)
    m_attackInfos.clear();
    m_etcAttackInfos.clear();
    m_intData.clear();
    m_keepDistance = -1;                     // movl $<A>,0x6e0
    m_isOnTheObject = false;                 // movb $0,0x6e4
    m_attackMethods.clear();                 // call(+0x6e8)
    m_waitingAction = "";                      // 六连展开 @0x5fc（实证非循环）
    m_moveAction = "";
    m_movebackAction = "";
    m_dashAction = "";
    m_dashattackAction = "";
    m_sitAction = "";
    for (i = 0; i <= 1; ++i)
        m_damageAction[i] = "";                  // 循环 ×2 @0x614
    m_downAction = "";                      // 四连展开 @0x61c
    m_overturnAction = "";
    m_jumpAction = "";
    m_jumpAttackAction = "";
    m_attackActions.clear();
    m_etcActions.clear();
    m_lastAction = "";                      // assign(+0x644)
    m_procAction = "";                      // assign(+0x648)
    m_patterns.clear();                      // vector<vector<int>> (+0x688)
    m_monsterTitle = "";                    // assign(+0x694)
    m_basicAction = "";
    m_damageSound = "";
    m_dieSound = "";
    m_appearSound = "";
    m_attackSound = "";
    m_ambientSound = "";
    m_etcSound = "";                       // assign(+0x6b0)
    m_field6f4 = 0;                          // +0x6f4
    m_field6f8 = 0;                          // +0x6f8
    m_dieEffect[0] = -1;                     // movl $<A>,0x6fc（{-1,0,0} 实证）
    m_dieEffect[1] = 0;                      // movl $0,0x700
    m_dieEffect[2] = 0;                      // movl $0,0x704
    m_field708 = "";                         // assign(+0x708)
    for (int k = 0; k < 111; ++k)            // clear_vs(this+12k+0x70c)
        m_field70c[k].clear();
    m_championElements.clear();              // call(+0xc40)
    m_fieldc4c = 0;                          // +0xc4c
    m_field088 = 0;                          // +0x88
    m_field08c.clear();                      // vector<pair>(+0x8c)
    m_fieldc54 = 0;                          // mov $0,%edx 中转形态
    m_fieldc58 = 0;
    m_field058.clear();                      // vector<pair>(+0x58)
    for (int l = 0; l <= 4; ++l)
        m_fieldc5c[l] = 0;                   // [0xc5c+4l]=0
    m_independentDrops.clear();              // call(+0x3ac)
    m_flagc70 = false;
    m_flagc71 = false;
    m_fieldc74 = 0;
    m_fieldc78 = 0;
    m_fieldc7c = 0;
    m_fieldc80.clear();
    m_fieldc8c.clear();
    m_dropAvatarInfo.clear();                // call stEnamyDropAvatarInfo_t::clear(+0xc98)
    m_flagcd4 = false;                       // movb $0,0xcd4
    m_field1c9c = 3;                         // movl $0x3,0x1c9c
    m_field1ca0 = 0;                         // movl $0x0,0x1ca0
    m_field1ca4 = 0;                         // mov $0,%edx 中转形态
    for (int m = 0; m < 200; ++m)            // difficultyScales 全表 -1.0f
        for (int n = 0; n <= 4; ++n)
            m_difficultyScales[m][n] = -1.0f;  // mov $0xbf800000（ebx 承载 n）
    m_field1c94 = 0;                         // +0x1c94
    m_field1c98 = 0;                         // +0x1c98
    m_field1ca8 = false;                     // movb $0,0x1ca8
    m_catchItems.clear();                    // call(+0x70)
    m_field1cac = -1;                        // movl $<A>,0x1cac
    m_field1cb0 = -1;                        // movl $<A>,0x1cb0
    m_field1cb4.clear();                     // call(+0x1cb4)
    m_mountingAction = "";                      // assign(+0x64c)
    m_mountedAction = "";                      // assign(+0x650)
    m_unmountingAction = "";                      // assign(+0x654)
    m_field658 = -1;                         // movl $<A>,0x658
    m_field65c = 0;
    m_field660 = 0;
    m_field664 = 0;
    m_field668 = 0;
    m_field66c = 0;
    m_field670 = 0;
    m_field674 = false;
    m_field678 = 255;                        // movl $0xff,0x678
    m_field67c = 255;
    m_field680 = false;
    m_field684 = 4;                          // movl $0x4,0x684
    m_fieldcd8 = 4;                          // movl $0x4,0xcd8
    m_field120.clear();                      // call(+0x120)
    m_baseParameters.clear();                // call(+0x12c)
    m_field1cc0 = 0;                         // movl $0,0x1cc0
