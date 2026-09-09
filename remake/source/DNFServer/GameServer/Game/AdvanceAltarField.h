// df_game_r advancealtar 共享叶子类型（无项目内依赖，仅 STL）。
// 证据：docs/class_func_reports/advancealtar__AdvanceAltarShopParameter/
//        {getFieldType,getdataType,importAdvanceAltarShopParameter}.md
//        docs/class_func_reports/advancealtar__RewardParameter/importAchievementRewardParameter.md
//        docs/class_func_reports/advancealtar__AchievementRewardData/AchievementRewardData.md
#ifndef DNF_GAME_ADVANCE_ALTAR_FIELD_H_
#define DNF_GAME_ADVANCE_ALTAR_FIELD_H_

#include <cstring>
#include <map>
#include <string>
#include <vector>

namespace advancealtar
{
// ---- advancealtar::AdvanceAltarShopType::T（ORIG mangled NS_20AdvanceAltarShopType1TE
// = advancealtar 内 struct 嵌 enum；ns 内 struct 版为唯一 mangled 正确形态）----
// 与 advancealtar.h 同守卫幂等副本（DNF_GAME_ADVANCE_ALTAR_SHOP_TYPE_NS_DEFINED，
// 2026-08-27 Altar 批统一：原 namespace 版是错误建模，已删除防 redefinition）。
#ifndef DNF_GAME_ADVANCE_ALTAR_SHOP_TYPE_NS_DEFINED
#define DNF_GAME_ADVANCE_ALTAR_SHOP_TYPE_NS_DEFINED
struct AdvanceAltarShopType { enum T { T_0 = 0, T_1 = 1, T_2 = 2 }; };
#endif

// ---- AdvanceAltarSlotType::T（ORIG mangled NS_20AdvanceAltarSlotType1TE；
// getSlotMap 仅认 1/2——枚举器名【推断】，与 advancealtar.h 同守卫幂等副本，
// 先例 ENUM_ERROR）----
#ifndef DNF_GAME_ADVANCE_ALTAR_SLOT_TYPE_DEFINED
#define DNF_GAME_ADVANCE_ALTAR_SLOT_TYPE_DEFINED
struct AdvanceAltarSlotType { enum T { SLOT_N1 = 1, SLOT_N2 = 2 }; };
#endif
// ---- FieldType::T：ORIG getFieldType(0x88a27a2) 名称→编号全表 ----
struct FieldType
{
    enum T
    {
        FT_LEVEL        = 0x00,  // "level"
        FT_INDEX        = 0x01,  // "index"
        FT_PRICE        = 0x02,  // "price"
        FT_NAME         = 0x03,  // "name"
        FT_COOLTIME     = 0x04,  // "cooltime"
        FT_NEED_POINT   = 0x05,  // "need point"
        FT_EXPLAIN      = 0x06,  // "explain"
        FT_TYPE_EXPLAIN = 0x07,  // "type explain"
        FT_SET_SLOT     = 0x08,  // "set slot"
        FT_IMAGE_INDEX  = 0x09,  // "image index"
        FT_HP           = 0x0a,  // "hp"
        FT_DMG          = 0x0b,  // "dmg"
        FT_PIC          = 0x0c,  // "pic"
        FT_RCV          = 0x0d,  // "rcv"
        FT_SUM          = 0x0e,  // "sum"
        FT_MHT          = 0x0f,  // "mht"
        FT_RGN          = 0x10,  // "rgn"
        FT_MSD          = 0x11,  // "msd"
        FT_ASD          = 0x12,  // "asd"
        FT_SDT          = 0x13,  // "sdt"
        FT_FDT          = 0x14,  // "fdt"
        FT_ETC          = 0x15,  // "etc"
        FT_LRG          = 0x16,  // "lrg"
        FT_SPL          = 0x17,  // "spl"
        FT_ETC2         = 0x18   // "etc2"
    };
};

// ORIG FieldData（C1 0x88a34fe W / D1 0x88a3820 W）：+0x00 int=-1、
// +0x04 int=0、+0x08 std::string，sizeof 0xc。
// importAdvanceAltarShopParameter 的 [field name]/[list] 段按
// getdataType(ft)==0 写 +8 字符串、否则写 +4 整数。
struct FieldData
{
    int m_field0;       // +0x00（ctor 置 -1）
    int m_value;        // +0x04（整型字段值）
    std::string m_str;  // +0x08（字符串字段值：getdataType==0 的 ft）

    FieldData() : m_field0(-1), m_value(0), m_str() { m_str.clear(); }  // ORIG 含 Ss::clear()
};

typedef std::map<FieldType::T, FieldData> FieldDataMap;
typedef std::map<int, FieldDataMap> LevelFieldDataMap;

// ---- _RewardItem（ORIG sizeof 0xc：ctor 0x889b9c0 W memset 0xc；vector 元素
// 尺寸实证 new_allocator<>::max_size 常量 0x15555555 = (2^32-1)/12）----
// 自 CDungeon.h 迁入收敛（advancealtar 命名空间共享类型统一本叶头）。
struct _RewardItem
{
    _RewardItem()  // ORIG C1/C2 W：memset(this,0,0xc)
    {
        memset(this, 0, sizeof(*this));
    }

    int m_type;      // +0x00（RewardItemType：0=normal 1=event）
    // b28d-solo-ProcStage（P170 c++filt 实证）：ORIG sendMailRewardItem 0x0812f5a4
    //   调 `_ZSt9make_pairIRKmRKiE…`（IRKm=const unsigned long&）⇒ 本字段真型
    //   unsigned long（32 位同宽；sizeof 0xc/偏移/mangled 零变）。
    unsigned long m_value1;    // +0x04
    int m_value2;    // +0x08（ORIG ImportAdvanceAltarDungeonTag 按 3 个 int 写入）
};

// ---- AchievementRewardData（ORIG C1 0x889ad06 T / D1 0x889adba T，布局取自
// ctor 与 importAchievementRewardParameter 0x889afb0 各段写入偏移；sizeof 0x28）。
// 注意：ORIG ctor 不初始化 +0xc（m_iconImage），本实现保持一致不触碰。
class AchievementRewardData
{
public:
    AchievementRewardData()
        : m_type(0), m_difficulty(-1), m_stageClearCount(0),
          m_rewardItems(), m_rewardTitle(), m_postalTitle(), m_postalLetter()
    {
    }
    ~AchievementRewardData() {}

    void reset();  // ORIG 0x0889ae56 T

    int m_type;                              // +0x00（map key；须 >0）
    int m_difficulty;                        // +0x04（StageDifficulty::T）
    int m_stageClearCount;                   // +0x08
    int m_iconImage;                         // +0x0c（ORIG ctor 不初始化）
    std::vector<_RewardItem> m_rewardItems;  // +0x10
    std::string m_rewardTitle;               // +0x1c
    std::string m_postalTitle;               // +0x20
    std::string m_postalLetter;              // +0x24
};  // sizeof 0x28

}  // namespace advancealtar

#endif  // DNF_GAME_ADVANCE_ALTAR_FIELD_H_
