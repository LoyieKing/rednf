#ifndef DNF_GAME_CHARAC_QUEST_SHOP_DATA_H_
#define DNF_GAME_CHARAC_QUEST_SHOP_DATA_H_

// ============================================================================
// CHARAC_QUEST_SHOP_DATA —— 商城购买状态条目（0xaa 字节，pack(1)）
// 自 DB_LoadEtc.h 抽出（SigTypes_D.h 的 SIG_QUEST_SHOP_DATA 双实例结构需完整类型，
// 且 DB_LoadEtc.h 依赖 SigTypes.h 存在包含环）。布局证据 = _LoadQuestShop
// （ORIG 0x08404b72）各 get_* 写点。ctor 定义于 DB_LoadEtc.cpp
// （ORIG W 0x0848f862，调 clear）。
// ============================================================================

// +0x0c..+0xa5 是数据库加载的连续 0x9a 字节区域；用具名嵌套类型保留
// 所有协议空洞，使清理路径可以对 m_data 整体操作而不再使用裸偏移。
struct CHARAC_QUEST_SHOP_DATA_FIELDS
{
    unsigned short m_psyAttack;              // +0x00
    unsigned short m_magAttack;              // +0x02
    unsigned short m_magDefence;             // +0x04
    unsigned short m_psyDefense;             // +0x06
    unsigned char  m_pad14[0x0c];             // parent +0x14..+0x1f
    unsigned short m_attackSpeed;            // parent +0x20
    unsigned short m_moveSpeed;              // parent +0x22
    unsigned char  m_pad24[2];               // parent +0x24
    unsigned short m_maxHp;                  // parent +0x26
    unsigned short m_hpRegen;                // parent +0x28
    unsigned short m_psyCritical;            // parent +0x2a
    unsigned short m_magCritical;            // parent +0x2c
    unsigned char  m_pad2e[6];               // parent +0x2e..+0x33
    unsigned short m_fireElementResist;      // parent +0x34
    unsigned short m_waterElementResist;     // parent +0x36
    unsigned short m_darkElementResist;      // parent +0x38
    unsigned short m_lightElementResist;     // parent +0x3a
    unsigned short m_allElementResist;       // parent +0x3c
    unsigned char  m_pad3e[4];               // parent +0x3e..+0x41
    unsigned short m_evasion;                // parent +0x42
    unsigned char  m_pad44[2];               // parent +0x44..+0x45
    unsigned short m_mpRegen;                // parent +0x46
    unsigned short m_hitRecovery;            // parent +0x48
    unsigned char  m_pad4a[4];               // parent +0x4a..+0x4d
    unsigned short m_goodHit;                // parent +0x4e
    unsigned char  m_pad50[0x10];             // parent +0x50..+0x5f
    unsigned short m_fireElementAttack;      // parent +0x60
    unsigned short m_waterElementAttack;     // parent +0x62
    unsigned short m_darkElementAttack;      // parent +0x64
    unsigned short m_lightElementAttack;     // parent +0x66
    unsigned short m_allElementAttack;       // parent +0x68
    unsigned char  m_pad6a[2];               // parent +0x6a..+0x6b
    unsigned short m_maxMp;                  // parent +0x6c
    unsigned char  m_pad6e[8];               // parent +0x6e..+0x75
    unsigned short m_separatePsyMagAttack;   // parent +0x76
    unsigned char  m_pad78[0x2e];            // parent +0x78..+0xa5
};
#pragma pack(push, 1)
struct CHARAC_QUEST_SHOP_DATA
{
    CHARAC_QUEST_SHOP_DATA();  // ORIG W 0x0848f862（调 clear）
    void clear();              // ORIG W 0x0848f83e（memset 0xaa）
    unsigned int   m_field0;                 // +0x000（未访问；前导）
    unsigned int   m_qp;                     // +0x004
    unsigned int   m_initCount;              // +0x008
    // b28c-L10：+0x0c..+0xa5 区双视角匿名 union。DB 侧（_LoadQuestShop）按属性具名字段
    // 填充，CQuestShop 侧（SetTagCharacQuestShopData ORIG 0x085efdc0）按 0x4d 个 ushort
    // 状态计数读取（`movzwl 0xc(%eax,%edx,2)`、make_pair 实例 `IRiRt` ⇒ 非 const 左值
    // 直访问完整类型成员）。0x4d*2 = 0x9a = sizeof(FIELDS)，sizeof/偏移/全 mangled 不变。
    // 探针 /tmp/l10_probe6.cpp fH：完整类型成员访问形逐条复现 ORIG，reinterpret_cast
    // 视图形使 4.4 装载序反（base 先 + 多 1 条 add）。
    union
    {
        CHARAC_QUEST_SHOP_DATA_FIELDS m_data;    // +0x00c..+0xa5
        // [L11·round3 实测回滚] 曾试 short→unsigned short（ORIG SetTag 的 make_pair 实例
        // 名为 `IRiRt`，我方 short 发 `IRiRs` ⇒ p167 MISSING/EXTRA 各 2）。改后 TU 读数
        // identical 89→87、合计 239→237（丢失 2 个共有符号：Rs 版实例不再发射）、
        // missing 67→62 ⇒ **净负**，已回滚。SetTag 本体在基线即为 identical/ae，
        // 非 near（worklist 的 NEAR 标记为旧读数）。
        short m_statusCounts[0x4d];               // 同区计数视角（b28d-36 试：异型化翻 store 序）
        // [round5-A26] 同址 ushort 视角（Main 配方⑲ / P235）：CQuestShop::loadData
        // ORIG 0x085efa9d 段取址形 `mov i,%eax; add %eax,%eax; add -0xc(%ebp),%eax;
        // lea 0xc(%eax),%edx` 且 make_pair 实例 `IRiRt`（非 const **unsigned short**
        // 左值）——整指针 reinterpret_cast 视图与元素 cast 两形实测均劣化（P235：
        // cast 视图形恒使 4.4 先载 base）。union 视角只增成员名，零符号/零布局影响。
        unsigned short m_statusCountsU[0x4d];
    };
    unsigned int   m_questPiece;             // +0x0a6
};
#pragma pack(pop)
static_assert(sizeof(CHARAC_QUEST_SHOP_DATA_FIELDS) == 0x9a,
              "CHARAC_QUEST_SHOP_DATA_FIELDS ABI drift");
static_assert(sizeof(CHARAC_QUEST_SHOP_DATA) == 0xaa,
              "CHARAC_QUEST_SHOP_DATA ABI drift");

#endif  // DNF_GAME_CHARAC_QUEST_SHOP_DATA_H_
