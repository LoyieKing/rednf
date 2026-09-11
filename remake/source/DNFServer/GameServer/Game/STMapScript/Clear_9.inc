// [b28d-solo-STMapScript] _ZN11STMapScript5ClearEv：p167 CLEAR 41/41（语义无缺口）；
// verdict DIFF 残差＝N82 槽复用（ORIG 两独立 pathgate 外层循环共用 i@-0x20，4.4 第二循环
// 用 -0x18）+ sightScroll 索引 add-vs-lea（P30 族）。
// 修法：①m_str1a0/m_str1b0 的 assign→operator=（ORIG _ZNSsaSEPKc，真调用目标修正）；
// ②pathgate 拆两个独立嵌套循环（ORIG 0x0836c91b/0x0836c963）；③`register int c`（P231）
// 复现 ORIG %ebx 缓存（push ebx;sub $0x34 已与 ORIG 同帧）。
void STMapScript::Clear()                   // ORIG T 0x0836c688（顺序保持汇编原序）
{
    m_maxMonsterDropException = 0;          // param_1[0xa1]=0
    m_vecBloodSystem.clear();               // +0xa2
    m_vecBloodPhaseTime.clear();            // +0xa5
    m_vecDefenseSystem.clear();             // +0xa8
    m_vecDefensePhaseTime.clear();          // +0xab
    m_summonStartArea.clear();              // MapArea::clear(+0x57)
    m_178 = 0;                              // param_1[0x5e]=0
    m_timeLine.reset();                     // advancealtar::TimeLine::reset(+0xcc)
    m_17c = 0;                              // param_1[0x5f]=0
    m_vec180.clear();                       // vector<pair>::clear(+0x60)
    m_list4.clear();                        // list<int>::clear(+1)
    m_c = 0;                                // param_1[3]=0
    m_recoveryObjects.clear();              // vector<pair>::clear(+4)
    m_listAnimation.clear();                // list<STAssignAnimation>::clear(+7)
    m_listQuestAnimation.clear();           // list<STQuestAnimation>::clear(+9)
    m_listPassiveObject.clear();            // list<STAssignPassiveObject>::clear(+0xb)
    m_vecMonster.clear();                   // vector<STAssignMonster>::clear(+0xd)
    m_vecConditionMonster.clear();          // vector<STAssignMonster>::clear(+0x10)
    m_championRate = 0;                     // param_1[0x13]=0
    m_listItem.clear();                     // list<STAssignItem>::clear(+0x14)
    m_listSpecialPassive.clear();           // list<STAssignSpecialPassiveObj>::clear(+0x16)
    m_element = 4;                          // param_1[0x1b]=4
    m_vecTile.clear();                      // vector<string>::clear(+0x1c)
    m_vecExtendedTile.clear();              // vector<string>::clear(+0x1f)
    // ORIG 0x0836c7f7：2×3 嵌套循环（外 -0x20≤1 双 init；内层 c 槽驻留 + body
    // 缓存 %ebx 为 4.1 寻址寄存器分配，4.4 不缓存 → 残余 N，助记符已全同）。
    // ORIG 全程复用同一循环变量槽 -0x20（17 次使用，跨 5 个循环）⇒ 原码声明一个
    // 函数级变量供各循环共用（4.4 对每个循环新声明会给独立槽，槽图不符）。
    // [判据⑩/判据⑪] ORIG 槽集 6 枚（shift 工具：ORIG-only=[-0x20,-0x1c]）：
    //   r@-0x20（sightScroll 外/blockPath/pathgate 两外循环，共 17 用）、
    //   c@-0x1c（sightScroll 内）、j1@-0x18（pathgatePos 内）、j2@-0x14（pathgateRange 内）、
    //   r2@-0x10（pvpStartArea）、r3@-0xc（keepStateAxis）。
    // 4.1 不复用（内层 j1/j2 与后两循环变量各占独立槽）⇒ 全部函数级具名，
    // 声明序按 ORIG 槽深（P331：同尺寸标量区先声明更深）。
    int r = 0;                               // [b28d-solo-STMapScript] register 提示实测反劣
    int c;                                   //（帧 0x34→0x20 + 多 push %esi）⇒ 回滚
    int j1;
    int j2;
    int r2;
    int r3;
    for (r = 0; r < 2; ++r)
        for (c = 0; c < 3; ++c)               // ORIG 内层占槽 -0x1c
            m_sightScroll[r * 3 + c] = 100; //  0x20+r*3+c word 基 0x22..0x27
    m_vecBackAni.clear();                   // vector<BackAniScript>::clear(+0x28)
    m_vecBackGroundEffect.clear();          // vector<BackGroundEffect>::clear(+0x2b)
    m_vecBlockEffect.clear();               // vector<BackGroundEffectBlock>::clear(+0x2e)
    m_vecDialog.clear();                    // vector<STDialog>::clear(+0x31)
    m_startPathDir = 4;                     // param_1[0x34]=4
    for (r = 0; r < 4; ++r)                 // byte(0xd4+i)=0（复用 -0x20）
        m_blockPath[r] = 0;
    m_tileOptionUpper = 0;                  // byte(param_1+0x36)=0
    m_vecTownMovableArea.clear();           // vector<TownPath>::clear(+0x37)
    m_vecVirtualMovableArea.clear();        // vector<MapArea>::clear(+0x3a)
    m_vecGuildAgitEntrance.clear();         // vector<TownPath>::clear(+0x3d)
    // ORIG 0x0836c91b / 0x0836c963：**两个独立嵌套循环**（各自 i@-0x20 复用，
    // j 分别 -0x18/-0x14），非单一外层包两内层。
    for (r = 0; r < 4; ++r)                 // 复用 -0x20
    {
        for (j1 = 0; j1 < 2; ++j1)
            m_pathgatePos[r * 2 + j1] = 0;  // 0x40+i*2+j word 基（ORIG j1@-0x18）
    }
    for (r = 0; r < 4; ++r)                 // 复用 -0x20
    {
        for (j2 = 0; j2 < 2; ++j2)
            m_pathgateRange[r * 2 + j2] = 0; // 0x48+i*2+j word 基（ORIG j2@-0x14）
    }
    m_dungeonStartArea.clear();             // MapArea::clear(+0x50)
    m_openingBgm.clear();                   // Ss::clear(+99=0x18c)
    m_openingBgmParam = 0;                  // param_1[100]=0(0x190)
    m_sortBottom = 0;                       // byte(param_1+0x6b)=0
    m_vecSound.clear();                     // vector<string>::clear(+0x65=0x194)
    // p167：ORIG 调 `_ZNSsaSEPKc`（string::operator=），非 assign（P167 调用目标）
    m_str1a0 = "";                          // Ss::operator=(+0x68,"")（UNK_08c2f620=""）
    // [N3Free171to190.S1STMapScript §6.5.1 编码盲区] ORIG .rodata 实字节
    //（file off 0xbce9d7 段）= `PVP \xb3\xeb\xb3\xd7\xc0\xd3`（EUC-KR，9 字节，含空格）；
    // 原写 UTF-8「PVP놀이게임」⇒ 发射字节不同（运行期差异）。按 ORIG 字节转义。
    m_str1b0 = "PVP \xb3\xeb\xb3\xd7\xc0\xd3";  // Ss::operator=(+0x6c)
    m_playerCountMin = 2;                   // param_1[0x6d]=2
    m_playerCountMax = 8;                   // param_1[0x6e]=8
    for (r2 = 0; r2 < 5; ++r2)              // 5×MapArea::clear(+0x6f+i*7 words)（ORIG r2@-0x10）
        m_pvpStartArea[r2].clear();
    m_24c = 0;                              // param_1[0x93]=0
    m_str1b0.clear();                       // Ss::clear(+0x6c)（汇编原序保留的重复清空）
    m_vecMonsterSpawnPos.clear();           // vector<STRivalMapData>::clear(+0x94)
    m_vecAICharacter.clear();               // vector<STAICharacterMapArrangeData>::clear(+0x97)
    m_apcRandomPoint = 0;                   // param_1[0x9a]=0
    m_mapLoadingImagePath.clear();          // Ss::clear(+0x9b)
    m_274 = 0;                              // param_1[0x9d]=0
    m_270 = m_274;                          // param_1[0x9c]=param_1[0x9d]
    m_vecEventMonsterPos.clear();           // vector<STRivalMapData>::clear(+0x9e)
    m_vecNpc.clear();                       // vector<AssignNpc>::clear(+0x18)
    m_index = 0;                            // *param_1=0
    m_vecAttackedMonster.clear();           // vector<STAttackedMonster>::clear(+0xc6)
    m_limitPlayerMoveAreas.clear();         // map<int,MapArea>::clear(+0xc0)
    m_maxMonsterCountException = 0;         // param_1[0xc9]=0
    m_328 = 0;                              // param_1[0xca]=0
    m_32c = 0;                              // param_1[0xcb]=0
    m_tournamentMonsters.clear();           // map<char,vector<TournamentMonster>>::clear(+0xae)
    m_moveLayeredType = 4;                  // param_1[0xd4]=4
    m_zposUnder = -1;                       // param_1[0xd5]=-1
    m_allDeadCasePassable = 0;              // byte(param_1+0xd6)=0
    m_screenEdgeLeft = -1;                  // param_1[0xd7]=-1
    m_screenEdgeRight = -1;                 // param_1[0xd8]=-1
    m_screenEdgeTop = -1;                   // param_1[0xd9]=-1
    m_screenEdgeBottom = -1;                // param_1[0xda]=-1
    m_blockUseActiveSkill = 0;              // byte(param_1+0xdb)=0
    m_blockUseStackableItem = 0;            // byte(0x36d)=0
    // [N3Free171to190.S1STMapScript 顺序轴] ORIG 0x0836c688 段序实证：
    //   0x36d → 0x248（m_pvpByPartySlot）→ loop 0x249+i（m_keepStateAxisX/Y/Z）
    //   → **0x36e（m_fixChampion）** → +0x370 clear。原码把 0x36e 放在循环**之前**
    //   ⇒ 三条指令位置与 ORIG 不符。按 ORIG 顺序下移（各赋值互相独立，零语义影响）。
    m_pvpByPartySlot = 0;
    for (r3 = 0; r3 < 3; ++r3)              // ORIG r3@-0xc
        (&m_keepStateAxisX)[r3] = 0;
    m_fixChampion = 0;                      // byte(0x36e)=0（ORIG 在 0x249 循环之后）
    m_str370.clear();                       // Ss::clear(+0xdc)
    m_visibleMinimap = 1;                   // param_1[0xdd]=1
    m_visibleTownMinimap = 1;               // param_1[0xde]=1
}