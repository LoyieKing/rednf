// df_game_r CEquipItem 还原（I1 批次，2026-08-16）。
// 函数形态以 docs/class_func_reports/CEquipItem.md + ORIG weak 反汇编为准
// （AE 口径：调用目标归一；字段偏移/常量逐条一致）。
// STEquipmentScript 内联完整布局为 I1 工作布局（脚本表批次细化），
// 依赖的辅助类（CodePage/CSystemTime/CMTRand 等）由 GameStubs.cpp 提供桩。

#include "CEquipItem.h"

#include <set>
#include <string.h>

#include "CSystemTime.h"
#include "GameTypes.h"

// ===================== 外部依赖（其它 TU 提供） =====================

int get_rand_int(int range);

class CEnvironment;
extern CEnvironment* G_CEnvironment();

class CDataManager
{
public:
    void set_AuctionRegFreeCeraItem(unsigned int idx);
};
extern CDataManager* G_CDataManager();

int getLevelLinearAbility(int a, int b, int c, int d, int level);

struct itemGloballyUniqueIdentifier_t
{
    void reset();
    char m_pad[0x11];
};

class CItemGloballyUniqueIdentifierGenerator
{
public:
    void generate(itemGloballyUniqueIdentifier_t* id, int field);
};

class CodePage
{
public:
    static const char* script();
    static const char* database();
    static bool script2Database(const char* src, char* dst);
};

namespace user_creature
{
class CCreatureScriptMgr
{
public:
    void RegisterCreatureIdAndItemId(int creatureId, int itemId);
};
CCreatureScriptMgr* GetInstanceCreatureScriptMgr();
}

namespace ExtreamDungeon
{
class CCompound_ExtreamDun_Item
{
public:
    void ForceResultItemRule_Setting(int idx, bool flag, int value);
};
}

// ===================== STEquipmentScript 工作布局（脚本表批次细化） =====================

class STEquipmentScript
{
public:
    STEquipmentScript();
    ~STEquipmentScript();

    int m_field0;                         // +0x00
    int m_field4;                         // +0x04
    int m_field8;                         // +0x08
    int m_fieldc;                         // +0x0c
    int m_field10;                        // +0x10
    int m_field14;                        // +0x14
    int m_field18;                        // +0x18
    int m_field1c;                        // +0x1c
    int m_field20;                        // +0x20
    int m_field24;                        // +0x24
    int m_field28;                        // +0x28
    int m_field2c;                        // +0x2c
    char m_field30;                       // +0x30
    bool m_field31;                       // +0x31
    char m_pad32[6];                      // +0x32
    bool m_usable[0xb];                   // +0x38
    char m_pad43[0x19];                   // +0x43
    int m_field5c;                        // +0x5c
    int m_field60;                        // +0x60
    int m_field64;                        // +0x64
    char m_pad68[4];                      // +0x68
    std::string m_str6c;                  // +0x6c
    char m_pad70[0x18];                   // +0x70
    std::vector<int> m_vec88;             // +0x88
    char m_pad94[0x18];                   // +0x94
    std::string m_strac;                  // +0xac
    char m_padb0[0x14];                   // +0xb0
    int m_fieldc4;                        // +0xc4
    int m_fieldc8;                        // +0xc8
    int m_fieldcc;                        // +0xcc
    bool m_fieldd0;                       // +0xd0
    char m_padd1[7];                      // +0xd1
    char m_fieldd8;                       // +0xd8
    bool m_fieldd9;                       // +0xd9
    bool m_fieldda;                       // +0xda
    char m_paddb[1];                      // +0xdb
    std::set<int> m_setdc;                // +0xdc
    int m_fieldf4;                        // +0xf4
    int m_fieldf8;                        // +0xf8
    char m_fieldfc;                       // +0xfc
    char m_padfd[3];                      // +0xfd
    int m_field100;                       // +0x100
    char m_field104;                      // +0x104
    STSpecialMonsterDrop m_specialDrop;   // +0x105..0x110
    char m_field10e;                      // +0x10e
    char m_field10f;                      // +0x10f
    std::vector<std::pair<int, int> > m_vec110;  // +0x110
    std::set<STItemScript::ENUM_ITEM_CATEGORY> m_set11c;  // +0x11c
    char m_pad134[4];                     // +0x134
    STItemScript::SEXPERTJOB::stExpertCompoundResultVariation
        m_compound138;                    // +0x138（8 字节）
    STItemScript::SEXPERTJOB::stExpertJobAdditionalExp
        m_additionalExp140;               // +0x140（8 字节）
    float m_field148;                     // +0x148
    float m_field14c;                     // +0x14c
    float m_field150;                     // +0x150
    float m_field154;                     // +0x154
    std::vector<STItemScript::SEXPERTJOB::SUSABLE> m_vec158;  // +0x158
    char m_pad164[0x14];                  // +0x164
    int m_field178;                       // +0x178
    int m_field17c;                       // +0x17c
    int m_field180;                       // +0x180
    int m_field184;                       // +0x184
    char m_pad188[0x2c];                  // +0x188
    int m_field1b4;                       // +0x1b4
    int m_field1b8;                       // +0x1b8
    int m_field1bc;                       // +0x1bc
    char m_pad1c0[0x28];                  // +0x1c0
    int m_field1e8;                       // +0x1e8
    int m_field1ec;                       // +0x1ec
    char m_pad1f0[0x234];                 // +0x1f0
    std::vector<char> m_vec424;           // +0x424
    char m_pad430[0x170];                 // +0x430
    int m_field5a0;                       // +0x5a0
    int m_field5a4;                       // +0x5a4
    char m_pad5a8[0x20];                  // +0x5a8
    int m_field5c8;                       // +0x5c8
    int m_field5cc;                       // +0x5cc
    char m_pad5d0[0x34];                  // +0x5d0
    int m_field604;                       // +0x604
    float m_field608;                     // +0x608
    int m_field60c;                       // +0x60c
    char m_pad610[0x8c];                  // +0x610
    char m_field69c;                      // +0x69c
    char m_pad6a0[0x1c];                  // +0x6a0
    int m_field6bc;                       // +0x6bc
    std::vector<stAvatarTypeSelect_t> m_vec6c0;  // +0x6c0
    std::vector<unsigned short> m_vec6cc;        // +0x6cc
    int m_field6d8;                       // +0x6d8
    char m_pad6dc[4];                     // +0x6dc
    int m_field6e0;                       // +0x6e0
    char m_pad6e4[0x10];                  // +0x6e4
    bool m_field6f4;                      // +0x6f4
    bool m_field6f5;                      // +0x6f5
    bool m_field6f6;                      // +0x6f6
    char m_pad6f7[1];                     // +0x6f7
    int m_field6f8;                       // +0x6f8
    std::map<ItemUpgradeType::T, std::pair<int, int> > m_map6fc;  // +0x6fc
    char m_pad714[0x24];                  // +0x714
    int m_field738;                       // +0x738
    char m_pad73c[0x14];                  // +0x73c
    int m_field750;                       // +0x750
    char m_pad754[0x10];                  // +0x754
    int m_field764;                       // +0x764
    char m_pad768[0x30];                  // +0x768
    int m_field798;                       // +0x798
    char m_pad79c[0x10];                  // +0x79c
    bool m_field7ac;                      // +0x7ac
    char m_pad7ad[0xf];                   // +0x7ad
    bool m_field7bc;                      // +0x7bc
    char m_pad7bd[0xf];                   // +0x7bd
    std::vector<char> m_vec7cc;           // +0x7cc
    std::vector<std::pair<int, int> > m_vec7d8;  // +0x7d8
    std::vector<std::pair<int, int> > m_vec7e4;  // +0x7e4
    char m_pad7f0[0x20];                  // +0x7f0
    int m_field810;                       // +0x810
    char m_field814;                      // +0x814
    char m_pad815[3];                     // +0x815
    int m_field818;                       // +0x818
    int m_field81c;                       // +0x81c
    int m_field820;                       // +0x820
    int m_field824;                       // +0x824
    int m_field828;                       // +0x828
    char m_pad82c[0x54];                  // +0x82c
    char m_field880;                      // +0x880
    char m_pad881[3];                     // +0x881
    int m_field884;                       // +0x884
    bool m_field888;                      // +0x888
    char m_field889;                      // +0x889
    char m_field88a;                      // +0x88a
    char m_field88b;                      // +0x88b
    int m_field88c;                       // +0x88c
    int m_field890;                       // +0x890
};

// 字段别名：脚本表批次细化前，0x105 起的 STSpecialMonsterDrop 区按成员访问
// （m_specialDrop 复盖 0x105..0x110，子字段即上述 0x105/109/10d/10e/10f）。

// ===================== 报告函数（CEquipItem.md） =====================

CEquipItem::CEquipItem() {}

CEquipItem::~CEquipItem() {}

unsigned char CEquipItem::IsAvatarColorVariation(int idx) const
{
    if (idx < 0 || idx > 1)
    {
        return 0;
    }
    return m_avatarColor[idx];
}

bool CEquipItem::IsAvatarOptionValid(char idx) const
{
    if (idx < 0)
    {
        return 0;
    }
    if ((signed char)m_avatarAbility.size() <= idx)
    {
        return 0;
    }
    return 1;
}

bool CEquipItem::checkItemUpgradePossible() const
{
    if (!IsImpossibleUpgrade())
    {
        if ((unsigned int)m_itemType < 0x16 &&
            ((1 << m_itemType) & 0x3ff400U) != 0)
        {
            return 1;
        }
    }
    return 0;
}

int CEquipItem::check_able_To_equip(char eType, int job, int level)
{
    if (is_stackable())
    {
        return 1;
    }
    if (!check_eType(eType))
    {
        return 0x17;
    }
    if (!check_job_type((char)job))
    {
        return 8;
    }
    if (!check_low_level(level))
    {
        return 0xe;
    }
    return 0;
}

bool CEquipItem::check_eType(char eType) const
{
    if (eType > 0x15)
    {
        return 0;
    }
    if ((char)m_itemType == eType)
    {
        return 1;
    }
    return 0;
}

ExpBonusPointData* CEquipItem::getExpBonusPoint() const
{
    return (ExpBonusPointData*)((char*)this + 0x1e8);
}

int CEquipItem::getGrowthGrade(int level) const
{
    if (m_growthFlag != 0)
    {
        return getLevelLinearAbility(m_growthA, m_growthB, m_growthC,
                                     m_growthD, level);
    }
    return m_grade;
}

int CEquipItem::getGrowthRepairCost(int level) const
{
    if (m_growthFlag != 0)
    {
        return getLevelLinearAbility(m_growthA, m_growthB, m_growthE,
                                     m_growthF, level);
    }
    return m_repairCost;
}

WpBonusPointData* CEquipItem::getWpBonusPoint() const
{
    return (WpBonusPointData*)((char*)this + 0x1e0);
}

int CEquipItem::get_aura_avatar_option_value()
{
    return m_auraOptionValue;
}

bool CEquipItem::isAmplifyPollutionItem() const
{
    return m_pollutionFlag;
}

bool CEquipItem::isExistUpgradableLevelTag()
{
    return m_upgradeLevel.size() != 0;
}

unsigned char CEquipItem::isGrowthType() const
{
    return m_growthFlag;
}

bool CEquipItem::isUpgradableLevel(ItemUpgradeType::T type, int level)
{
    std::map<ItemUpgradeType::T, std::pair<int, int> >::iterator it =
        m_upgradeLevel.find(type);
    if (it != m_upgradeLevel.end())
    {
        if (level < it->second.first || it->second.second < level)
        {
            return 0;
        }
    }
    return 1;
}

bool CEquipItem::is_aura_avatar_option(char option)
{
    if ((unsigned char)m_auraOptionType == (char)option)
    {
        return 1;
    }
    return 0;
}

bool CEquipItem::verify()
{
    if (m_usablePeriod != 0 || m_expirationDate != 0)
    {
        if ((GetItemType() < 0 || GetItemType() > 9) &&
            GetItemType() != 0xb &&
            GetItemType() != 0x16 &&
            GetItemType() != 10)
        {
            return 0;
        }
    }
    return 1;
}

// ===================== make_item（ORIG 0x851098a） =====================

void CEquipItem::make_item(Inven_Item& item) const
{
    if (IsAvatarItem())
    {
        item.m_field1 = 8;
    }
    else
    {
        item.m_field1 = 1;
    }
    if (m_itemType == 0x16)
    {
        item.m_field1 = 5;
    }
    else if (m_itemType == 0x17 || m_itemType == 0x18 || m_itemType == 0x19)
    {
        item.m_field1 = 6;
    }
    item.set_add_info(get_rand_int(0));
    item.ResetItemAttr();
    item.m_fieldb = (unsigned short)m_randomTable.m_endurance;
    if (item.m_field1 == 8)
    {
        if (m_avatarSelectIdx > 0 && m_avatarSelectIdx <
            (int)((std::vector<stAvatarTypeSelect_t>*)getAvatarTypeSelect())->size())
        {
            item.m_fieldb = (unsigned short)m_avatarSelectIdx;
        }
    }
    if (m_attachType == 3)
    {
        item.m_field0 = 1;
    }
    if (PeriodAfterUnsealing() == 0)
    {
        int t = 0;
        if (getUsablePeriod() != 0)
        {
            t = ((CSystemTime*)0x941f714)->getCurSec() +
                getUsablePeriod() * 0x15180;
        }
        else if (getExpirationDate() != 0)
        {
            t = getExpirationDate();
        }
        if (t != 0 && m_itemType != 10)
        {
            item.m_fieldb = (unsigned short)((t - 0x44a54a80) / 0x15180);
        }
    }
    item.m_fieldd = 0;
    ((itemGloballyUniqueIdentifier_t*)((char*)&item + 0x15))->reset();
    if ((unsigned int)m_itemType < 0x1a &&
        ((1 << m_itemType) & 0x3bffc00U) != 0)
    {
        ((CItemGloballyUniqueIdentifierGenerator*)0x9494d68)
            ->generate((itemGloballyUniqueIdentifier_t*)((char*)&item + 0x15),
                       *(int*)((char*)G_CEnvironment() + 0x198));
    }
    item.m_amp.reset();
}

// ===================== set_item（ORIG 0x850fd84） =====================

void CEquipItem::set_item(STEquipmentScript& script)
{
    char buf[500];

    m_index = script.m_field0;
    m_grade = script.m_field4;
    m_field0c = script.m_field14;
    m_field30 = script.m_field24;
    m_field10 = script.m_field18;
    m_field14 = script.m_field1c;
    m_rarity = script.m_field10;
    m_char60 = script.m_fieldfc;
    m_pair24.first = script.m_field28;
    m_pair24.second = script.m_field2c;
    if (script.m_field30)
    {
        G_CDataManager()->set_AuctionRegFreeCeraItem(m_index);
    }
    m_boolF8 = script.m_field31;
    __asm__ __volatile__(
        "movl $0, %%eax\n\t"
        "movl $0x7d, %%edx\n\t"
        "movl %0, %%edi\n\t"
        "movl %%edx, %%ecx\n\t"
        "rep stosl\n\t"
        : : "b"(buf) : "eax", "edx", "ecx", "edi", "memory");
    strcpy(buf, script.m_strac.c_str());
    if (!CodePage::script2Database(script.m_strac.c_str(), buf))
    {
        cMyTrace trace("void CEquipItem::set_item(STEquipmentScript&)", 0x553,
                       5);
        trace("Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
              CodePage::database(), script.m_strac.c_str(),
              "void CEquipItem::set_item(STEquipmentScript&)", 0x553);
    }
    m_str2c = buf;
    m_attachType = script.m_field8;
    m_field50 = script.m_fieldc;
    m_field64 = script.m_field20;
    m_field68 = script.m_field88c;
    for (int i = 0; i <= 10; ++i)
    {
        m_jobUsable[i] = script.m_usable[i];
    }
    m_usableLevel = script.m_field5c;
    m_field4c = script.m_field60;
    m_itemType = script.m_field1b4;
    m_randomTable.m_endurance = script.m_field1bc;
    m_stackable = 0;
    m_int58 = script.m_field184;
    m_int5c = script.m_field180;
    m_randomTable.m_field10 = script.m_field750;
    m_randomTable.m_field14 = script.m_field6bc;
    m_randomTable.m_field20 = script.m_field738;
    if (script.m_field1b4 == 0x16)
    {
        switch (script.m_field1b8)
        {
        case 0:
            user_creature::GetInstanceCreatureScriptMgr()
                ->RegisterCreatureIdAndItemId(m_randomTable.m_field14, m_index);
            break;
        case 1:
            m_randomTable.m_field1c = script.m_field764;
            break;
        case 2:
            m_randomTable.Set(&script.m_vec88);
            break;
        }
    }
    m_randomTable.m_field18 = script.m_field1b8;
    m_usablePeriod = script.m_fieldf4;
    m_expirationDate = script.m_fieldf8;
    m_randomTable.m_field24 = script.m_field7ac;
    m_avatarSelect = script.m_vec6c0;
    m_vec180 = script.m_vec6cc;
    m_field18c = script.m_field6d8;
    m_repairCost = script.m_field178;
    m_itemGroupName = script.m_field100;
    m_char78 = script.m_field104;
    m_bool79 = script.m_field7bc;
    m_strb0 = script.m_str6c;
    m_int80 = script.m_fieldc8;
    m_int7c = script.m_fieldcc;
    m_impossibleUpgrade = script.m_field6f4;
    m_impossibleAmplify = script.m_field6f5;
    m_impossibleDisjoint = script.m_field6f6;
    m_impossibleGift = script.m_fieldd0;
    m_impossibleCharacCargo = script.m_fieldda;
    m_possibleKiriProtection = script.m_fieldd9;
    m_periodAfterUnsealing = script.m_field6f8;
    m_drop = script.m_specialDrop;
    script.m_specialDrop.clear();
    m_char194 = script.m_field10e;
    m_vec198 = script.m_vec7d8;
    m_vec1a4 = script.m_vec7e4;
    m_enchantVec = script.m_vec7cc;
    m_field1bc = 0;
    if (IsAvatarItem())
    {
        if (m_grade == 3 && script.m_field798 == 3)
        {
            m_field1bc = 6;
        }
        setAvatarOptionSize((int)script.m_vec424.size());
    }
    else if (GetItemType() == 0x16)
    {
        if (script.m_field10 == 2)
        {
            m_field1bc = 10;
        }
    }
    else if (script.m_field10 == 2)
    {
        if (script.m_fieldc4 == 5)
        {
            m_field1bc = 2;
        }
        else if (script.m_fieldc4 == 0x1e)
        {
            m_field1bc = 1;
        }
        else
        {
            m_field1bc = 9;
        }
    }
    else if (script.m_field10 == 3)
    {
        m_field1bc = 3;
    }
    else if (script.m_field10 == 5)
    {
        m_field1bc = 4;
    }
    else if (script.m_field10 == 4)
    {
        m_field1bc = 5;
    }
    else if (script.m_field10 == 0)
    {
        m_field1bc = 7;
    }
    else if (script.m_field10 == 1)
    {
        m_field1bc = 8;
    }
    m_dropBD = script.m_field10f;
    m_vecC0 = script.m_vec110;
    m_questBonus = script.m_field810;
    m_field1c0 = script.m_field17c;
    if (script.m_field814 != 2)
    {
        ((ExtreamDungeon::CCompound_ExtreamDun_Item*)0x941f750)
            ->ForceResultItemRule_Setting(m_index, script.m_field814 != 0,
                                          script.m_field818);
    }
    m_setd4 = script.m_setdc;
    m_intEC = script.m_field64;
    m_avatarSelectIdx = script.m_field6e0;
    m_randomOption = script.m_field888;
    m_randomGrade = script.m_field889;
    m_floatFC = script.m_field150;
    m_float100 = script.m_field154;
    m_vec104.clear();
    m_vec104 = script.m_vec158;
    m_compound110 = script.m_compound138;
    m_float118 = script.m_field148;
    m_float11c = script.m_field14c;
    m_additionalExp120 = script.m_additionalExp140;
    m_set128 = script.m_set11c;
    m_field1cc = script.m_field604;
    m_field1d0 = (int)script.m_field608;
    m_field1d4 = script.m_field60c;
    m_auraOptionType = script.m_field880;
    m_auraOptionValue = script.m_field884;
    int uVar4 = script.m_field820;
    m_field1e0 = script.m_field81c;
    m_field1e4 = uVar4;
    uVar4 = script.m_field828;
    m_field1e8 = script.m_field824;
    m_field1ec = uVar4;
    m_avatarAbility = script.m_vec424;
    if (script.m_field890 == 1)
    {
        m_pollutionFlag = 0;
    }
    else
    {
        m_pollutionFlag = 1;
    }
    m_upgradeLevel = script.m_map6fc;
    m_avatarColor[0] = script.m_field88a;
    m_avatarColor[1] = script.m_field88b;
    m_growthFlag = script.m_field69c;
    m_growthA = script.m_field1e8;
    m_growthB = script.m_field1ec;
    m_growthC = script.m_field5a0;
    m_growthD = script.m_field5a4;
    m_growthE = script.m_field5c8;
    m_growthF = script.m_field5cc;
    m_char144 = script.m_fieldd8;
    m_parentBoosterItemId = 0;
}

// ===================== 简单虚函数 / 访问器（ORIG weak） =====================

int CEquipItem::GetItemType() const
{
    return m_itemType;
}

void CEquipItem::setAvatarOptionSize(int size)
{
    m_avatarOptionSize = size;
}

bool CEquipItem::IsAvatarItem() const
{
    if (m_itemType >= 0 && m_itemType <= 9)
    {
        return 1;
    }
    return 0;
}

bool CEquipItem::IsCreatureItem() const
{
    if (m_itemType > 0x15 && m_itemType <= 0x19)
    {
        return 1;
    }
    return 0;
}

bool CEquipItem::IsClearAvatar() const
{
    return m_randomTable.m_field24;
}

bool CEquipItem::IsEnchantalbe() const
{
    if (m_itemType > 9 && m_itemType <= 0x15)
    {
        return 1;
    }
    return 0;
}

bool CEquipItem::getUsableItemType(int type) const
{
    return 0;
}

bool CEquipItem::isEnableAvatarFunc(ENUM_AVATAR_FUNC_FILTER filter) const
{
    return (m_field18c & (int)filter) == (int)filter;
}

int CEquipItem::GetGuildPowerWarPoint() const
{
    return m_int80;
}

int CEquipItem::GetUserPowerWarPoint() const
{
    return m_int7c;
}

bool CEquipItem::IsImpossibleUpgrade() const
{
    return m_impossibleUpgrade;
}

bool CEquipItem::IsImpossibleAmplify() const
{
    return m_impossibleAmplify;
}

bool CEquipItem::IsImpossibleDisjoint() const
{
    return m_impossibleDisjoint;
}

bool CEquipItem::IsImpossibleGift() const
{
    return m_impossibleGift;
}

bool CEquipItem::IsRandomOption() const
{
    return m_randomOption;
}

int CEquipItem::GetRandomOptionGrade() const
{
    return (signed char)m_randomGrade;
}

void CEquipItem::set_parent_booster_item_id(unsigned long id)
{
    m_parentBoosterItemId = id;
}

unsigned long CEquipItem::get_parent_booster_item_id() const
{
    return m_parentBoosterItemId;
}

int CEquipItem::get_endurance() const
{
    return m_randomTable.m_endurance;
}

void* CEquipItem::getAvatarTypeSelect() const
{
    return (void*)((char*)this + 0x174);
}
