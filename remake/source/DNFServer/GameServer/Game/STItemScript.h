#ifndef GAME_STITEMSCRIPT_H_
#define GAME_STITEMSCRIPT_H_

// ============================================================================
// df_game_r 还原 —— STItemScript（0x178 布局）
// 布局依据 ORIG 构造（0x898d3c6）/ Clear（0x898d718）/ 析构（0x89c36f0）
// 逐成员推导；SEXPERTJOB/STSpecialMonsterDrop/DyeInfo 的实现在其自身 TU
// （ORIG 0x89c36aa / 0x8513a10 / 0x8513ac6），此处仅声明以保持调用形态。
// CItem 也依赖本头的 DyeInfo/STSpecialMonsterDrop/SEXPERTJOB 嵌套类型，
// 因此这些公共类型定义在本头（CItem.h 通过 include 本头获得）。
// ============================================================================

#include <set>
#include <string>
#include <utility>
#include <vector>

// ---- [war room point bonus] / [war room monster exp bonus] 载体（8 字节，int type/int value）。
// [b28d-solo-CEquipItem] 原分处 CEquipItem.h（WpBonusPointData/ExpBonusPointData）与
// STEquipmentScript.h（stBonusPointData_t 同布局副本）；ORIG nm 只有
// WpBonusPointData/ExpBonusPointData 两个符号，且 STEquipmentScript::C1（0x0898ddd0/dde0）
// 在 +0x81c/+0x824 直接 call `_ZN16WpBonusPointDataC1Ev` / `_ZN17ExpBonusPointDataC1Ev`
// ⇒ 两处本为同一类型。按 §4.4 合并到本公共头（CEquipItem.h 与 STEquipmentScript.h
// 共同 include 链），删除 stBonusPointData_t 重复定义。布局/符号名不变。
struct WpBonusPointData
{
    // ORIG W ctor 0x08513a6e（独立符号，调 out-of-line clear()）→ ctor/clear 均 out-of-line。
    WpBonusPointData();
    void clear();
    int m_type;     // +0x00（0=基础 / 1=附加）
    int m_value;    // +0x04
};

struct ExpBonusPointData
{
    // ORIG W ctor 0x08513a9a（独立符号，调 out-of-line clear()）→ ctor/clear 均 out-of-line。
    ExpBonusPointData();
    void clear();
    int m_type;     // +0x00（0=基础 / 1=附加）
    int m_value;    // +0x04
};

// ---- 全局专家职业类型（ORIG 全局 enum，mangled E20ENUM_EXPERT_JOB_TYPE） ----

enum ENUM_EXPERT_JOB_TYPE
{
    ENUM_EXPERT_JOB_TYPE_0 = 0,
    ENUM_EXPERT_JOB_TYPE_1 = 1,
    ENUM_EXPERT_JOB_TYPE_2 = 2,
    ENUM_EXPERT_JOB_TYPE_3 = 3,
    ENUM_EXPERT_JOB_TYPE_4 = 4,
    ENUM_EXPERT_JOB_TYPE_5 = 5,
    ENUM_EXPERT_JOB_TYPE_6 = 6,
    ENUM_EXPERT_JOB_TYPE_7 = 7,
    ENUM_EXPERT_JOB_TYPE_8 = 8
};

#pragma pack(push, 1)
class STSpecialMonsterDrop
{
public:
    STSpecialMonsterDrop();
    void clear();
    // ORIG（readCondtion/ImportSpecialMonsterDrop 族按字节写 +0x105..+0x10d）：
    // 该区是 9 个独立 flag 字节，不是 {int,int,char}。总大小保持 9。
    bool m_enable;          // +0x00（ORIG +0x105：[special monster drop] 开关；
                            //  bool：ORIG getter 0x8539e00 `movzbl; ret` 无归一，P292）
    // bool（P292 双向直证）：getter 定义体 0x8539e10/0x8539e24 `movzbl; ret` 无归一，
    // 且调用点 0x8534ee4/0x8534ee7 `call; xor $1; test %al; jne` ⇒ 返回 bool；
    // bool 字段+bool 返回 = 7 条无 setne（char 字段+bool 返回会多 test/setne 两条）。
    // sizeof/偏移/mangled 零变化（bool 仍 1 字节）。
    bool m_difficulty[5];   // +0x01..+0x05（easy/medium/hard/ultimate/hero）
    bool m_dungeonType[3];  // +0x06..+0x08（normal/ancient/dimension）
};
#pragma pack(pop)

class DyeInfo
{
public:
    DyeInfo();
    ~DyeInfo();
    void clear();
    DyeInfo& operator=(const DyeInfo& other);
    int getDyeColorIndex();                 // ORIG 0x898d37c _ZN7DyeInfo16getDyeColorIndexEv
    // [Lane41 · Main 批准] 两成员 private→public：仅访问控制变化，成员类型/声明
    // 顺序不变 ⇒ sizeof(16)/offsetof(+0x00/+0x04)/mangled 零变化。
    // 依据：ORIG ImportStackableScript 0x089b9601 段对 +0x164 `mov %edx,0x164(%eax)`、
    // 0x089b9629 `lea 0x168(%eax),%ecx`（[dye type]/[dye info]）直访成员，消费侧据此
    // 删除 TU 局部 DyeInfoView + reinterpret_cast 视图 cast 反模式。
    int m_field0;                            // +0x00
    std::vector<std::pair<int, int> > m_vec4;  // +0x04（vector<pair<int,int>>）
};

class STItemScript
{
public:
    enum ENUM_ITEM_CATEGORY
    {
        ITEM_CATEGORY_0 = 0,
        ITEM_CATEGORY_1 = 1,
        ITEM_CATEGORY_2 = 2,
        ITEM_CATEGORY_3 = 3,
        ITEM_CATEGORY_4 = 4,
        ITEM_CATEGORY_5 = 5
    };

    struct SEXPERTJOB
    {
        struct SUSABLE
        {
            ENUM_EXPERT_JOB_TYPE m_field0;  // +0x00（专家职业类型）
            int m_field4;                   // +0x04（等级下限）
        };

        struct stExpertJobAdditionalExp
        {
            stExpertJobAdditionalExp();
            float m_field0;  // +0x00
            unsigned int m_field4;    // +0x04（P211 异型化：int→uint 翻 store 序，同宽偏移不变）
        };

        struct stExpertCompoundResultVariation
        {
            stExpertCompoundResultVariation();
            float m_field0;  // +0x00
            float m_field4;  // +0x04
        };

        SEXPERTJOB();
        ~SEXPERTJOB();

        // ORIG 布局：+0x00 为 stExpertCompoundResultVariation（8B）、
        // +0x08 为 stExpertJobAdditionalExp（8B），ctor 分别调用。
        stExpertCompoundResultVariation m_variation;   // +0x00
        stExpertJobAdditionalExp m_additionalExp;      // +0x08
        float m_field10;               // +0x10
        float m_field14;               // +0x14
        float m_field18;               // +0x18
        float m_field1c;               // +0x1c
        std::vector<SUSABLE> m_usable; // +0x20
    };

    STItemScript();
    ~STItemScript();
    void Clear();
    bool hasCategory(ENUM_ITEM_CATEGORY category) const;

    int m_field0;                    // +0x00
    int m_field4;                    // +0x04
    int m_field8;                    // +0x08
    int m_fieldc;                    // +0x0c
    int m_field10;                   // +0x10
    int m_field14;                   // +0x14
    int m_field18;                   // +0x18
    int m_field1c;                   // +0x1c
    int m_field20;                   // +0x20
    int m_field24;                   // +0x24
    std::pair<int, int> m_pair28;    // +0x28
    char m_field30;                  // +0x30
    bool m_field31;                  // +0x31（CEquipItem::set_item 直拷到 m_boolF8 实证 bool）
    int m_field34;                   // +0x34
    // [b28d-solo-CEquipItem] ORIG set_item 0x0850fe60 段 `movzbl 0x38(%ecx,%edx,1),%ecx;
    // mov %cl,dst` 对 bool 目标直存（无 test/setne）⇒ 源数组元素为 bool（P136/P300）。
    // 与同结构 m_field31 已 bool 化同判据；宽度/偏移/sizeof/mangled 不变。
    bool m_usable[0xb];              // +0x38
    std::set<int> m_set44;           // +0x44
    int m_field5c;                   // +0x5c
    int m_field60;                   // +0x60
    int m_field64;                   // +0x64
    int m_field68;                   // +0x68
    std::string m_str6c;             // +0x6c
    int m_field70;                   // +0x70
    std::string m_str74;             // +0x74
    int m_field78;                   // +0x78
    std::string m_str7c;             // +0x7c
    int m_field80;                   // +0x80
    std::string m_str84;             // +0x84
    std::vector<int> m_vec88;        // +0x88
    std::vector<std::string> m_vec94;// +0x94
    char m_fielda0;                  // +0xa0
    int m_fielda4;                   // +0xa4
    int m_fielda8;                   // +0xa8
    std::string m_strac;             // +0xac
    std::string m_strb0;             // +0xb0
    std::string m_strb4;             // +0xb4
    std::string m_strb8;             // +0xb8
    std::string m_strbc;             // +0xbc
    std::string m_strc0;             // +0xc0
    int m_fieldc4;                   // +0xc4
    int m_fieldc8;                   // +0xc8
    int m_fieldcc;                   // +0xcc
    bool m_fieldd0;                  // +0xd0
    char m_fieldd1;                  // +0xd1
    int m_fieldd4;                   // +0xd4
    // [C04 2026-09-10] `char`→`bool`（必配对律第三查点：目标字段 ↔ 写入源字段同型化）：
    //   ORIG 两写点直证为**无归一化直存** —— CEquipItem::set_item 0x085107ef
    //   `movzbl 0xd8(%eax),%edx` → 0x085107f9 `mov %dl,0x144(%eax)`；CStackableItem::set_item
    //   0x0850e825 `movzbl 0xd8(%eax),%edx` → 0x0850e82f `mov %dl,0x144(%eax)`
    //   （同段 0x0850e816 `setne %dl` 属**另一字段**的表达式物化）。目标端 `m_char144`
    //   已为 bool（E01，CItem.h）⇒ 源端不成对时 4.4 必插 `test/setne`。
    //   ABI：1 字节同宽 ⇒ sizeof/全部成员偏移/对齐/mangled 零变化；
    //   邻字段 m_fieldd9/m_fieldda 本就是 bool（同结构先例）。
    bool m_fieldd8;                  // +0xd8（"ban redeemitem" 标记，STStackableScript.cpp:3443 置 1）
    bool m_fieldd9;                  // +0xd9
    bool m_fieldda;                  // +0xda
    std::set<int> m_setdc;           // +0xdc
    int m_fieldf4;                   // +0xf4
    int m_fieldf8;                   // +0xf8
    char m_fieldfc;                  // +0xfc
    int m_field100;                  // +0x100
    char m_field104;                 // +0x104
    STSpecialMonsterDrop m_specialDrop;  // +0x105（9 字节）
    char m_field10e;                     // +0x10e
    bool m_field10f;                     // +0x10f
    std::vector<std::pair<int, int> > m_vec110;  // +0x110
    std::set<ENUM_ITEM_CATEGORY> m_set11c;       // +0x11c
    unsigned short m_field134;       // +0x134
    SEXPERTJOB m_expertJob;          // +0x138
    DyeInfo m_dyeInfo;               // +0x164
    std::string m_str174;            // +0x174
};

// [Lane41] DyeInfo 零布局锁（private→public 改动不触布局；此断言防漂移）：
// int + vector<pair<int,int>> = 4 + 12 = 16 字节；成员序固定 ⇒ m_vec4 必在 +0x04
// （+0x164 整体偏移的 +0x168 直访见 ORIG 0x089b9629 `lea 0x168(%eax),%ecx`）。
// 注：不用 offsetof(DyeInfo, m_vec4) —— gcc4.4 对非 POD 成员取址恒告警
// （invalid access to non-static data member），保留 sizeof 断言已锁死布局。
// 证据：ORIG DyeInfo ctor 0x8513ac6 / clear 0x898d718 / getDyeColorIndex 0x898d37c 族。
static_assert(sizeof(DyeInfo) == 16, "DyeInfo size drift");

#endif  // GAME_STITEMSCRIPT_H_
