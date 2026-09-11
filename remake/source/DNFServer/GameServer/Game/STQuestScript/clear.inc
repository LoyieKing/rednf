void QuestScript::clear()
{
    m_index = -1;                    // +0x00
    m_name = "";                     // +0x04
    m_requestDefault = "";           // +0x08
    m_conditionDefault = "";         // +0x0c
    m_solveDefault = "";             // +0x10
    m_fieldC8.clear();               // +0xc8
    m_str2e8 = "";                   // +0x2e8（ORIG 顺序：早于 m_questType）
    m_2ec = 0;                       // +0x2ec
    m_questType = 4;                 // +0xd4
    m_npcIdx = 0;                    // +0xe4
    m_fieldE8 = -1;                  // +0xe8
    m_field134 = -1;                 // +0x134
    m_jobChangeQuest = 0;            // +0xdc
    m_field1b0.clear();              // +0x1b0
    m_field1bc = 0;                  // +0x1bc
    m_field1c0 = -1;                 // +0x1c0
    m_postalReward.clear();          // +0x298
    m_field1c4 = 4;                  // +0x1c4
    m_targetCharacters.clear();      // +0xf4
    m_fieldE0 = -1;                  // +0xe0
    m_fieldD8 = 3;                   // +0xd8
    m_fieldEC = 0xb;                 // +0xec
    m_fieldF0 = -1;                  // +0xf0
    for (int i = 0; i <= 1; ++i)
        m_levelMin[i] = 0;           // +0x100/+0x104
    m_field108 = 0;                  // +0x108
    m_intDataList.clear();           // +0x10c
    m_antiQuestList.clear();         // +0x118
    m_questCountLimit = 0;           // +0x124
    m_field130 = 0;                  // +0x130
    m_clearItems.clear();            // +0x138
    m_killItems.clear();             // +0x144
    m_appearMap.clear();             // +0x150
    m_rewardSelectList.clear();      // +0x160
    m_triggerType = 0;               // +0x178
    m_triggerSubType = 0;            // +0x17c
    m_triggerData.clear();           // +0x180
    m_field18c.clear();              // +0x18c
    m_rewardType = 0;                // +0x1d4
    m_titleReward.clear();           // +0x1d8
    m_field1e4 = -1;                 // +0x1e4
    m_field228 = -1;                 // +0x228
    m_field20c.clear();              // +0x20c
    m_field218 = 0;                  // +0x218
    m_rewardData.clear();            // +0x21c
    m_field22c = 0;                  // +0x22c
    m_field23d = 0;                  // +0x23d
    m_field240 = 100;                // +0x240
    m_str244 = "";                   // +0x244
    m_field248 = -1;                 // +0x248
    m_field24c = -1;                 // +0x24c
    m_field250 = -1;                 // +0x250
    m_field254 = -1;                 // +0x254
    m_mailQuestFlag = -1;            // +0x258
    m_mailTitle.clear();             // +0x25c
    m_mailContents.clear();          // +0x260
    m_mailItems.clear();             // +0x264
    m_field1f4.clear();              // +0x1f4
    m_selectReward.clear();          // +0x1e8
    m_eventDialogs.clear();          // +0x200
    m_field22d = 1;                  // +0x22d
    m_field22e = 0;                  // +0x22e
    for (int i = 0; i <= 2; ++i)
        m_autoDisplay.m_values[i] = -1;          // +0x230..+0x23b
    m_field23c = 0;                  // +0x23c
    m_field270 = 0;                  // +0x270
    m_field271 = 1;                  // +0x271
    m_powerSide = 0xff;              // +0x272
    m_field274 = -1;                 // +0x274
    m_field278 = 0;                  // +0x278
    m_field27c = 0;                  // +0x27c
    m_questIdx = 0;                  // +0x14
    for (int i = 0; i <= 10; ++i)
    {
        m_jobNames[i] = 0;            // +0x18
        m_jobRequests[i] = 0;         // +0x44
        m_jobConditions[i] = 0;       // +0x70
        m_jobSolves[i] = 0;           // +0x9c
    }
    m_field280 = 0;                  // +0x280
    m_field282 = 0;                  // +0x282
    m_field284 = -1;                 // +0x284
    m_field288 = -1;                 // +0x288
    m_field28c = -1;                 // +0x28c
    m_field290 = -1;                 // +0x290
    m_str294.clear();                // +0x294
    m_field2a8 = 0;                  // +0x2a8
    m_field2ac = -1;                 // +0x2ac
    m_2c8 = 0;                       // +0x2c8
    m_str2cc = "";                   // +0x2cc
    m_exceptionList.clear();         // +0x2d0
    m_field128 = 0;                  // +0x128
    m_field12c = 0;                  // +0x12c
    m_dailySchedule.clear();         // +0x2dc
    m_field12c = 0;                  // +0x12c
    m_field128 = 0;                  // +0x128
    m_vec2f0.clear();                // +0x2f0
    m_vec2fc.clear();                // +0x2fc
    m_308 = 0;                       // +0x308
}