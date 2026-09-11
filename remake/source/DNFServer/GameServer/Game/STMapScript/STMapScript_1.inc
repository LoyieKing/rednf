// [b28d-solo-STMapScript] _ZN11STMapScriptC1ERKS_：p167 CLEAR 95/95（语义无缺口）；
// verdict DIFF 残差＝N90 数组成员块搬移分块启发式（ORIG 5+4+1+1 组间重载基址，4.4 单次连发）。
// 修法：m_pvpStartArea 进初始化列表（复现 ORIG 0x0837eb82 倒计数 C1ERKS_×5；原为
// C1Ev×5+aSERKS_×5）。
// [N3Free171to190.S1STMapScript 收尾判词·N] 尾部 ORIG 独有 9 段 EH cleanup 模板（每段
// `mov 0x8(%ebp),%eax; add $0xNN,%eax; mov %eax,(%esp); call <T>` + `mov %esi,%eax;
// mov %ebx,%edx; jmp <T>`），我方合并为单次 ⇒ N104（EH 尾块相对序）+ N90（ORIG 43 条
// 准备段 vs 我方 7），源码不可控。
STMapScript::STMapScript(const STMapScript& other)      // ORIG C1 0x0837e698
    : m_index(other.m_index),                           // *param_1 = *param_2
      m_list4(other.m_list4),                           // list<int> copy
      m_c(other.m_c),                                   // param_1[3]
      m_recoveryObjects(other.m_recoveryObjects),       // vector<pair>@0x10
      m_listAnimation(other.m_listAnimation),           // list@0x1c
      m_listQuestAnimation(other.m_listQuestAnimation), // list@0x24
      m_listPassiveObject(other.m_listPassiveObject),   // list@0x2c
      m_vecMonster(other.m_vecMonster),                  // vector@0x34
      m_vecConditionMonster(other.m_vecConditionMonster), // vector@0x40
      m_championRate(other.m_championRate),             // param_1[0x13]
      m_listItem(other.m_listItem),                     // list@0x50
      m_listSpecialPassive(other.m_listSpecialPassive), // list@0x58
      m_vecNpc(other.m_vecNpc),                         // vector@0x60
      m_element(other.m_element),                       // param_1[0x1b]
      m_vecTile(other.m_vecTile),                       // vector<string>@0x70
      m_vecExtendedTile(other.m_vecExtendedTile),       // vector<string>@0x7c
            m_vecBackAni(other.m_vecBackAni),                 // vector@0xa0
      m_vecBackGroundEffect(other.m_vecBackGroundEffect), // vector@0xac
      m_vecBlockEffect(other.m_vecBlockEffect),         // vector@0xb8
      m_vecDialog(other.m_vecDialog),                   // vector@0xc4
      m_startPathDir(other.m_startPathDir),             // param_1[0x34]
            m_tileOptionUpper(other.m_tileOptionUpper),       // byte @0xd8
      m_vecTownMovableArea(other.m_vecTownMovableArea), // TownPath@0xdc
      m_vecVirtualMovableArea(other.m_vecVirtualMovableArea), // MapArea@0xe8
      m_vecGuildAgitEntrance(other.m_vecGuildAgitEntrance),   // TownPath@0xf4
                  m_dungeonStartArea(other.m_dungeonStartArea),     // MapArea@0x140
      m_summonStartArea(other.m_summonStartArea),       // MapArea@0x15c
      m_178(other.m_178),                               // param_1[0x5e]
      m_17c(other.m_17c),                               // param_1[0x5f]
      m_vec180(other.m_vec180),                         // vector<pair>@0x180
      m_openingBgm(other.m_openingBgm),                 // string@99(0x18c)
      m_openingBgmParam(other.m_openingBgmParam),       // param_1[100](0x190)
      m_vecSound(other.m_vecSound),                     // vector<string>@0x194
      m_str1a0(other.m_str1a0),                         // string@0x68(0x1a0)
      m_str1a4(other.m_str1a4),                         // string@0x69(0x1a4)
      m_str1a8(other.m_str1a8),                         // string@0x6a(0x1a8)
      m_sortBottom(other.m_sortBottom),                 // byte @0x6b(0x1ac)
      m_str1b0(other.m_str1b0),                         // string@0x6c(0x1b0)
      m_playerCountMin(other.m_playerCountMin),         // param_1[0x6d]/[0x6e] 成对
      m_playerCountMax(other.m_playerCountMax),
      m_pvpStartArea(other.m_pvpStartArea),             // MapArea[5]@0x1bc（ORIG C1ERKS_×5 倒计数）
            m_pvpByPartySlot(other.m_pvpByPartySlot),         // byte @0x92(0x248)
      m_keepStateAxisX(other.m_keepStateAxisX),         // u16 @0x249 成对
      m_keepStateAxisY(other.m_keepStateAxisY),
      m_keepStateAxisZ(other.m_keepStateAxisZ),         // byte @0x24b
            m_vecMonsterSpawnPos(other.m_vecMonsterSpawnPos), // vector@0x94(0x250)
      m_vecAICharacter(other.m_vecAICharacter),         // vector@0x97(0x25c)
      m_apcRandomPoint(other.m_apcRandomPoint),         // param_1[0x9a]
      m_mapLoadingImagePath(other.m_mapLoadingImagePath), // string@0x9b(0x26c)
      m_270(other.m_270),                               // param_1[0x9c]/[0x9d] 成对
      m_274(other.m_274),
      m_vecEventMonsterPos(other.m_vecEventMonsterPos), // vector@0x9e(0x278)
      m_maxMonsterDropException(other.m_maxMonsterDropException), // param_1[0xa1]
      m_vecBloodSystem(other.m_vecBloodSystem),         // vector@0xa2(0x288)
      m_vecBloodPhaseTime(other.m_vecBloodPhaseTime),   // vector@0xa5(0x294)
      m_vecDefenseSystem(other.m_vecDefenseSystem),     // vector@0xa8(0x2a0)
      m_vecDefensePhaseTime(other.m_vecDefensePhaseTime), // vector@0xab(0x2ac)
      m_tournamentMonsters(other.m_tournamentMonsters), // map@0xae(0x2b8)
      m_tournamentStartPos1(other.m_tournamentStartPos1), // vector@0xb4(0x2d0)
      m_tournamentStartPos2(other.m_tournamentStartPos2), // vector@0xb7(0x2dc)
      m_vecStaticStartPos(other.m_vecStaticStartPos),   // vector@0xba(0x2e8)
      m_vecStaticStartPos2(other.m_vecStaticStartPos2), // vector@0xbd(0x2f4)
      m_limitPlayerMoveAreas(other.m_limitPlayerMoveAreas), // map@0xc0(0x300)
      m_vecAttackedMonster(other.m_vecAttackedMonster), // vector@0xc6(0x318)
      m_maxMonsterCountException(other.m_maxMonsterCountException), // param_1[0xc9]
      m_328(other.m_328),                               // param_1[0xca]
      m_32c(other.m_32c),                               // param_1[0xcb]
      m_timeLine(other.m_timeLine),                     // TimeLine@0xcc(0x330)
      m_str34c(other.m_str34c),                         // string@0xd3(0x34c)
      m_moveLayeredType(other.m_moveLayeredType),       // param_1[0xd4]/[0xd5] 成对
      m_zposUnder(other.m_zposUnder),
      m_allDeadCasePassable(other.m_allDeadCasePassable), // byte @0xd6(0x358)
      m_screenEdgeLeft(other.m_screenEdgeLeft),         // param_1[0xd7](0x35c)
      m_screenEdgeRight(other.m_screenEdgeRight),       // param_1[0xd8](0x360)
      m_screenEdgeTop(other.m_screenEdgeTop),           // param_1[0xd9](0x364)
      m_screenEdgeBottom(other.m_screenEdgeBottom),     // param_1[0xda](0x368)
      m_blockUseActiveSkill(other.m_blockUseActiveSkill), // byte @0xdb(0x36c)
      m_blockUseStackableItem(other.m_blockUseStackableItem), // byte 0x36d
      m_fixChampion(other.m_fixChampion),               // byte 0x36e
      m_pad36f(0),
      m_str370(other.m_str370),                         // string@0xdc(0x370)
      m_visibleMinimap(other.m_visibleMinimap),         // param_1[0xdd](0x374)
      m_visibleTownMinimap(other.m_visibleTownMinimap)  // param_1[0xde](0x378)
{
    // 标量区补拷贝（copy 伪 C 中间段）：
    for (int i = 0; i < 6; ++i)                         // param_1[0x22..0x27]=0x88..0x9f
        m_sightScroll[i] = other.m_sightScroll[i];
    for (int i = 0; i < 4; ++i)                         // param_1[0x35]+i = 0xd4..0xd7
        m_blockPath[i] = other.m_blockPath[i];
    for (int i = 0; i < 8; ++i)                         // param_1[0x40..0x4f]=0x100..0x13f
    {
        m_pathgatePos[i] = other.m_pathgatePos[i];
        m_pathgateRange[i] = other.m_pathgateRange[i];
    }
}