void STAICharacterCommonScript::clear()
{
    // ORIG 0x088c9592（objdump 逐条转写，保持调用次序与默认值）。
    m_minimumInfo.clear();               // MinimumInfo(+4).clear
    m_status.Clear();                    // STRDCharacterStatus(+0xc).Clear
    m_equipment.clear();                 // +0x64
    m_skills[0].clear();                 // +0x70
    m_skills[1].clear();                 // +0x7c
    m_corrections.clear();               // +0x88
    m_items[0].clear();                  // +0x94
    m_items[1].clear();                  // +0xa0
    m_info.clear();                      // +0xac
    m_keyStreamFiles.clear();            // +0x2d0
    m_keyFiles.clear();                  // +0x2dc
    m_keyCooltimes.clear();              // +0x2e8
    m_rarity = 0;                        // +0xd80
    m_appearancePoint = 0;               // +0xd84
    m_deathTowerItems.clear();           // +0xd88
    m_partyMembers.clear();              // +0xd94
    m_farAttackReactionRate = 0;         // +0xda0
    m_pvpWinPointLow = 0;                // +0xda4
    m_pvpWinPointHigh = 0;               // +0xda8
    m_championElements.clear();          // +0xf20（ORIG 先于 StatusRate 清理）
    m_statusRate.Clear();                // +0xdac
    m_customSkills.clear();              // +0xe50
    m_attackDamageRate = 1.0f;           // +0xe5c
    m_addEquipStatusFromLevel = 0;       // +0xe60
    m_armorSubtype = 2;                  // +0xe64
    m_defaultEquipment = 1;              // +0xe68
    m_dollSpecialDungeons.clear();       // +0xe6c
    m_specialStatusRate.Clear();         // +0xe78
    m_attackDamageRateSpecial = 1.0f;    // +0xf1c
    m_independentDrops.clear();          // +0xf2c
    m_onAttackSpeech.clear();            // +0xd68
    m_onKeySpeech.clear();               // +0xd74
    m_limitPassiveObjects.clear();       // +0xf38
    m_overheadGaugeType = 0;             // +0xf44
    m_overheadGaugePosX = 0;             // +0xf48
    m_overheadGaugePosY = 0;             // +0xf4c
    m_avatarDrop.clear();                // +0xf50
    m_getupInvincible = 1;               // +0xf8c = 1
    m_skillDataUps.clear();              // +0xf90
    m_summonStatusRates.clear();         // +0xf9c
    m_leaveBody = true;                  // byte +0xfa8 = 1
    m_addGravityInAerial = false;        // byte +0xff4 = 0
    m_gravityAerialA = 20.0f;            // +0xff8 = 0x41a00000
    m_gravityAerialB = -300.0f;          // +0xffc = 0xc3960000（-300.0）
    // ORIG 0x088c9819..0x088c9898 逐条序：1000 → 1008 → 1004 → POD 组
    // （POD 写序 0,1,7,4,6,5,2,3 非单调——原码语句序照抄）→ 100c → 1010。
    m_noCooltime = 0;                    // +0x1000
    m_npcId = -1;                        // +0x1008
    m_checkAccountItem = true;           // byte +0x1004 = 1
    m_info.m_pod204[0] = 0;
    m_info.m_pod204[1] = 0;
    m_info.m_pod204[7] = 0;
    m_info.m_pod204[4] = 0;
    m_info.m_pod204[6] = 0;
    m_info.m_pod204[5] = 0;
    m_info.m_pod204[2] = 0;
    m_info.m_pod204[3] = 0;
    m_isHellMonster = false;             // byte +0x100c = 0
    m_animationJob = 11;                 // +0x1010 = 0xb
}