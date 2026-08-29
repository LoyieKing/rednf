#ifndef DNF_GAME_CHARAC_QUEST_SHOP_DATA_H_
#define DNF_GAME_CHARAC_QUEST_SHOP_DATA_H_

// ============================================================================
// CHARAC_QUEST_SHOP_DATA —— 商城购买状态条目（0xaa 字节，pack(1)）
// 自 DB_LoadEtc.h 抽出（SigTypes_D.h 的 SIG_QUEST_SHOP_DATA 双实例结构需完整类型，
// 且 DB_LoadEtc.h 依赖 SigTypes.h 存在包含环）。布局证据 = _LoadQuestShop
// （ORIG 0x08404b72）各 get_* 写点。ctor 定义于 DB_LoadEtc.cpp
// （ORIG W 0x0848f862，调 clear）。
// ============================================================================

#pragma pack(push, 1)
struct CHARAC_QUEST_SHOP_DATA
{
    CHARAC_QUEST_SHOP_DATA();  // ORIG W 0x0848f862（调 clear）
    void clear();              // ORIG W 0x0848f83e（memset 0xaa）
    unsigned int   m_field0;                 // +0x000（未访问；前导）
    unsigned int   m_qp;                     // +0x004
    unsigned int   m_initCount;              // +0x008
    unsigned short m_psyAttack;              // +0x00c
    unsigned short m_magAttack;              // +0x00e
    unsigned short m_magDefence;             // +0x010
    unsigned short m_psyDefense;             // +0x012
    unsigned char  m_pad14[0x20 - 0x14];     // +0x014
    unsigned short m_attackSpeed;            // +0x020
    unsigned short m_moveSpeed;              // +0x022
    unsigned char  m_pad24[2];               // +0x024
    unsigned short m_maxHp;                  // +0x026
    unsigned short m_hpRegen;                // +0x028
    unsigned short m_psyCritical;            // +0x02a
    unsigned short m_magCritical;            // +0x02c
    unsigned char  m_pad2e[0x34 - 0x2e];     // +0x02e
    unsigned short m_fireElementResist;      // +0x034
    unsigned short m_waterElementResist;     // +0x036
    unsigned short m_darkElementResist;      // +0x038
    unsigned short m_lightElementResist;     // +0x03a
    unsigned short m_allElementResist;       // +0x03c
    unsigned char  m_pad3e[4];               // +0x03e
    unsigned short m_evasion;                // +0x042
    unsigned char  m_pad44[2];               // +0x044
    unsigned short m_mpRegen;                // +0x046
    unsigned short m_hitRecovery;            // +0x048
    unsigned char  m_pad4a[4];               // +0x04a
    unsigned short m_goodHit;                // +0x04e
    unsigned char  m_pad50[0x60 - 0x50];     // +0x050
    unsigned short m_fireElementAttack;      // +0x060
    unsigned short m_waterElementAttack;     // +0x062
    unsigned short m_darkElementAttack;      // +0x064
    unsigned short m_lightElementAttack;     // +0x066
    unsigned short m_allElementAttack;       // +0x068
    unsigned char  m_pad6a[2];               // +0x06a
    unsigned short m_maxMp;                  // +0x06c
    unsigned char  m_pad6e[0x76 - 0x6e];     // +0x06e
    unsigned short m_separatePsyMagAttack;   // +0x076
    unsigned char  m_pad78[0xa6 - 0x78];     // +0x078
    unsigned int   m_questPiece;             // +0x0a6
};
#pragma pack(pop)
static_assert(sizeof(CHARAC_QUEST_SHOP_DATA) == 0xaa,
              "CHARAC_QUEST_SHOP_DATA ABI drift");

#endif  // DNF_GAME_CHARAC_QUEST_SHOP_DATA_H_
