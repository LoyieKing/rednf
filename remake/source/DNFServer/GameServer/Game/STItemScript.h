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
    int m_field0;    // +0x00
    int m_field4;    // +0x04
    char m_field8;   // +0x08
};
#pragma pack(pop)

class DyeInfo
{
public:
    DyeInfo();
    ~DyeInfo();
    void clear();
    DyeInfo& operator=(const DyeInfo& other);
private:
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
            int m_field4;    // +0x04
        };

        struct stExpertCompoundResultVariation
        {
            stExpertCompoundResultVariation();
            float m_field0;  // +0x00
            float m_field4;  // +0x04
        };

        SEXPERTJOB();
        ~SEXPERTJOB();

        float m_field0;                // +0x00 (stExpertCompoundResultVariation.f0)
        float m_field4;                // +0x04
        float m_field8;                // +0x08 (stExpertJobAdditionalExp.f0)
        int m_fieldc;                  // +0x0c
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
    char m_field31;                  // +0x31
    int m_field34;                   // +0x34
    char m_usable[0xb];              // +0x38
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
    char m_fieldd0;                  // +0xd0
    char m_fieldd1;                  // +0xd1
    int m_fieldd4;                   // +0xd4
    char m_fieldd8;                  // +0xd8
    char m_fieldd9;                  // +0xd9
    char m_fieldda;                  // +0xda
    std::set<int> m_setdc;           // +0xdc
    int m_fieldf4;                   // +0xf4
    int m_fieldf8;                   // +0xf8
    char m_fieldfc;                  // +0xfc
    int m_field100;                  // +0x100
    char m_field104;                 // +0x104
    STSpecialMonsterDrop m_specialDrop;  // +0x105（9 字节）
    char m_field10e;                     // +0x10e
    char m_field10f;                     // +0x10f
    std::vector<std::pair<int, int> > m_vec110;  // +0x110
    std::set<ENUM_ITEM_CATEGORY> m_set11c;       // +0x11c
    unsigned short m_field134;       // +0x134
    SEXPERTJOB m_expertJob;          // +0x138
    DyeInfo m_dyeInfo;               // +0x164
    std::string m_str174;            // +0x174
};

#endif  // GAME_STITEMSCRIPT_H_
