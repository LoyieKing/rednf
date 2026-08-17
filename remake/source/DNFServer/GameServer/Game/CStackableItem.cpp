// df_game_r CStackableItem 还原（I1 批次，2026-08-17）。
// 函数形态以 docs/class_func_reports/CStackableItem.md + ORIG weak 反汇编为准
// （AE 口径：调用目标归一；字段偏移/常量逐条一致）。
// 依赖的辅助类（CMTRand/CSystemTime/CDataManager 等）由 GameStubs.cpp 提供桩；
// STStackableScript 内联完整布局为 I1 工作布局（脚本表批次细化）。

#include "CStackableItem.h"

#include <algorithm>
#include <string.h>
#include <time.h>

#include "CSystemTime.h"
#include "GameTypes.h"

// ===================== 外部依赖（其它 TU 提供） =====================

// 最小构造/析构（ORIG 逐成员构造属后续精修批次；保证可链接）
CStackableItem::CStackableItem()
{
}

CStackableItem::~CStackableItem()
{
}

int get_rand_int(int range);

enum ENUM_ITEM_CREATE_TYPE
{
    ENUM_ITEM_CREATE_TYPE_0 = 0,
    ENUM_ITEM_CREATE_TYPE_1 = 1
};

class CItemList
{
public:
    void create_item(ENUM_ITEM_CREATE_TYPE type, Inven_Item& item, int param);
};

class CEnvironment;
extern CEnvironment* G_CEnvironment();

struct CEnvironmentView
{
    char m_pad[0x1a8];
    int m_serverType;   // +0x1a8
};

class CDataManager
{
public:
    void set_AuctionRegFreeCeraItem(unsigned int idx);
    void set_lottery_use_cost(unsigned int cost);
    char m_pad[0xc];
    class CItemList* m_itemList;   // +0x0c
};
extern CDataManager* G_CDataManager();

struct CDataManagerView
{
    char m_pad[0x28];
    int m_field28;   // +0x28
};

class CMTRand
{
public:
    CMTRand(const unsigned long& seed);
    int randInt(const unsigned long& range);
};

class CSystemTime;
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

class GameWorld;
extern GameWorld* G_GameWorld();

class GameWorld
{
public:
    bool IsPVPChannel() const;
};

class CUserCharacInfo;
class CUser
{
public:
    int get_state();
    CUserCharacInfo* get_charac_info();
};

class CUserCharacInfo
{
public:
    char getCurCharFirstGrowType() const;
};

int GetEquipmentTypeStringToEnum(std::string& type);

void createRecipeInfo(RecipeInfo* info, std::vector<int>* source,
                      std::vector<std::pair<int, int> >* extra);
void createLotteryInfo(LotteryInfo* info, std::vector<int>* source);
void createStackableLotteryInfo(LotteryInfo* info, std::vector<int>* source);
void createStackableMultiBoxLotteryInfo(
    MultiBoxLotteryInfo::MultiBoxLotteryInfoBody* info,
    std::vector<std::vector<int> >* a, std::vector<int>* b,
    std::vector<int>* c);
void createGlobalEffectInfo(std::vector<char>& out, std::vector<int>* source);

class CChattingEmoticonList
{
public:
    void* find_emoticon(int idx);
};

// 调用 ORIG DyeInfo::operator=（成员拷贝语义；GameStubs 提供桩）
void DyeInfo_assign(DyeInfo& dst, const DyeInfo& src)
    __asm__("_ZN7DyeInfoaSERKS_");

// ===================== STStackableScript 工作布局（脚本表批次细化） =====================

class STStackableScript
{
public:
    STStackableScript();
    ~STStackableScript();
    int m_field0;                        // +0x00
    int m_field4;                        // +0x04
    int m_field8;                        // +0x08
    int m_fieldc;                        // +0x0c
    int m_field10;                       // +0x10
    int m_field14;                       // +0x14
    int m_field18;                       // +0x18
    int m_field1c;                       // +0x1c
    int m_field20;                       // +0x20
    int m_field24;                       // +0x24
    int m_field28;                       // +0x28
    int m_field2c;                       // +0x2c
    char m_field30;                      // +0x30
    bool m_field31;                      // +0x31
    char m_pad32[6];                     // +0x32
    bool m_usable[0xb];                  // +0x38
    char m_pad43[0x19];                  // +0x43
    int m_field5c;                       // +0x5c
    int m_field60;                       // +0x60
    int m_field64;                       // +0x64
    int m_field68;                       // +0x68
    std::string m_str6c;                 // +0x6c
    char m_pad70[0x18];                  // +0x70
    std::vector<int> m_vec88;            // +0x88
    std::vector<std::string> m_vec94;    // +0x94
    char m_pada0[0xc];                   // +0xa0
    std::string m_strac;                 // +0xac
    char m_padb0[0x14];                  // +0xb0
    int m_fieldc4;                       // +0xc4
    int m_fieldc8;                       // +0xc8
    int m_fieldcc;                       // +0xcc
    bool m_fieldd0;                      // +0xd0
    char m_padd1[7];                     // +0xd1
    bool m_fieldd8;                      // +0xd8
    bool m_fieldd9;                      // +0xd9
    bool m_fieldda;                      // +0xda
    char m_paddb[1];                     // +0xdb
    std::set<int> m_setdc;               // +0xdc
    int m_fieldf4;                       // +0xf4
    int m_fieldf8;                       // +0xf8
    char m_fieldfc;                      // +0xfc
    char m_padfd[3];                     // +0xfd
    int m_field100;                      // +0x100
    char m_field104;                     // +0x104
    STSpecialMonsterDrop m_specialDrop;  // +0x105
    char m_field10e;                     // +0x10e
    char m_field10f;                     // +0x10f
    std::vector<std::pair<int, int> > m_vec110;  // +0x110
    std::set<STItemScript::ENUM_ITEM_CATEGORY> m_set11c;  // +0x11c
    char m_pad134[0x1c];                 // +0x134
    int m_field150;                      // +0x150
    int m_field154;                      // +0x154
    std::vector<STItemScript::SEXPERTJOB::SUSABLE> m_vec158;  // +0x158
    DyeInfo m_dye164;                    // +0x164
    char m_pad174[4];                    // +0x174
    int m_field178;                      // +0x178
    char m_field17c;                     // +0x17c
    char m_pad17d[3];                    // +0x17d
    std::string m_str180;                // +0x180
    std::string m_str184;                // +0x184
    std::string m_str188;                // +0x188
    int m_field18c;                      // +0x18c
    std::vector<int> m_vec190;           // +0x190
    std::vector<char> m_vec19c;          // +0x19c
    char m_pad1a8[4];                    // +0x1a8
    int m_field1ac;                      // +0x1ac
    int m_field1b0;                      // +0x1b0
    int m_field1b4;                      // +0x1b4
    char m_pad1b8;                       // +0x1b8
    bool m_field1b9;                     // +0x1b9
    bool m_field1ba;                     // +0x1ba
    bool m_field1bb;                     // +0x1bb
    bool m_field1bc;                     // +0x1bc
    bool m_field1bd;                     // +0x1bd
    bool m_field1be;                     // +0x1be
    bool m_field1bf;                     // +0x1bf
    char m_field1c0;                     // +0x1c0
    char m_pad1c1[3];                    // +0x1c1
    int m_field1c4;                      // +0x1c4
    char m_pad1c8[8];                    // +0x1c8
    bool m_field1d0;                     // +0x1d0
    char m_pad1d1[0xf];                  // +0x1d1
    int m_field1e0;                      // +0x1e0
    int m_field1e4;                      // +0x1e4
    int m_field1e8;                      // +0x1e8
    std::vector<int> m_vec1ec;           // +0x1ec
    std::vector<StackableAction::UsablePlace> m_vec1f8;  // +0x1f8
    char m_pad204;                       // +0x204
    char m_field205;                     // +0x205
    char m_pad206[0xe];                  // +0x206
    std::vector<StackableAction::UsableServerType> m_vec214;  // +0x214
    int m_field220;                      // +0x220
    std::vector<int> m_vec224;           // +0x224
    char m_pad230[4];                    // +0x230
    std::vector<std::vector<int> > m_vec234;  // +0x234
    char m_pad240[4];                    // +0x240
    std::vector<std::vector<int> > m_vec244;  // +0x244
    std::vector<int> m_vec250;           // +0x250
    std::vector<int> m_vec25c;           // +0x25c
    std::vector<char> m_vec268;          // +0x268
    std::vector<std::vector<char> > m_vec274;  // +0x274
    stStackableBooster_t m_booster;      // +0x280
    std::vector<char> m_vec2a4;          // +0x2a4
    char m_pad2b0[0x7c];                 // +0x2b0
    char m_field32c;                     // +0x32c
    char m_pad32d[0x2b];                 // +0x32d
    int m_field358;                      // +0x358
    char m_pad35c[4];                    // +0x35c
    char m_field360;                     // +0x360
    char m_pad361[3];                    // +0x361
    unsigned short m_field364;           // +0x364
    char m_pad366[0x1a];                 // +0x366
    unsigned short m_field380;           // +0x380
    char m_pad382[2];                    // +0x382
    char m_pad384[0x10];                 // +0x384
    unsigned short m_field394;           // +0x394
    char m_field396;                     // +0x396
    char m_pad397;                       // +0x397
    unsigned short m_field398;           // +0x398
    char m_field39a;                     // +0x39a
    char m_field39b;                     // +0x39b
    int m_field39c;                      // +0x39c
    char m_field3a0;                     // +0x3a0
    char m_field3a1;                     // +0x3a1
    char m_pad3a2[2];                    // +0x3a2
    std::map<int, int> m_map3a4;         // +0x3a4
    char m_pad3bc[4];                    // +0x3bc
    std::vector<std::pair<int, int> > m_vec3c0;  // +0x3c0
    int m_field3cc;                      // +0x3cc
    char m_field3d0;                     // +0x3d0
    char m_field3d1;                     // +0x3d1
    char m_pad3d2[2];                    // +0x3d2
    std::vector<std::pair<int, int> > m_vec3d4;  // +0x3d4
    int m_field3e0;                      // +0x3e0
    int m_field3e4;                      // +0x3e4
    std::vector<stLimitCubeResult> m_vec3e8;     // +0x3e8
    std::map<int, int> m_map3f4;         // +0x3f4
    char m_field40c;                     // +0x40c
    char m_fields40d[2];                 // +0x40d
    char m_pad40f[9];                    // +0x40f
    int m_field418;                      // +0x418
    BoosterSelectInfo m_boosterSelect;   // +0x41c
    char m_field438;                     // +0x438
    char m_pad439[3];                    // +0x439
    int m_field43c;                      // +0x43c
    int m_field440;                      // +0x440
    int m_field444;                      // +0x444
    int m_field448;                      // +0x448
    int m_field44c;                      // +0x44c
    int m_field450;                      // +0x450
    int m_field454;                      // +0x454
    int m_field458;                      // +0x458
    int m_field45c;                      // +0x45c
    int m_field460;                      // +0x460
    int m_field464;                      // +0x464
    int m_field468;                      // +0x468
    int m_field46c;                      // +0x46c
    char m_pad470[4];                    // +0x470
    int m_field474;                      // +0x474
    int m_field478;                      // +0x478
    unsigned short m_field47c;           // +0x47c
    char m_field47e;                     // +0x47e
    char m_pad47f;                       // +0x47f
    std::set<int> m_set480;              // +0x480
    std::set<int> m_set498;              // +0x498
    bool m_field4b0;                     // +0x4b0
    char m_pad4b1[7];                    // +0x4b1
    int m_field4b8;                      // +0x4b8
    char m_pad4bc[0x30];                 // +0x4bc
    int m_field4ec;                      // +0x4ec
    char m_pad4f0[0x18];                 // +0x4f0
    int m_field508;                      // +0x508
    std::set<int> m_set50c;              // +0x50c
};

// ===================== 报告函数（CStackableItem.md） =====================

bool CStackableItem::IsAvatarOptionChangeItem() const
{
    return m_avatarOptionChange;
}

int CStackableItem::check_able_To_use(int job, int level)
{
    if (!check_job_type((char)job))
    {
        return 1;
    }
    if (!check_low_level(level))
    {
        return 2;
    }
    return 0;
}

bool CStackableItem::check_change_job_type(char changeJob,
                                           char checkJob) const
{
    if (!check_job_type(checkJob))
    {
        return 0;
    }
    if ((char)m_growType < 0)
    {
        return 1;
    }
    return m_growType == changeJob;
}

void CStackableItem::getBoosterItem(
    std::vector<stStackableBoosterItemInfo_t>& out) const
{
    for (std::vector<stStackableBoosterElement_t>::const_iterator it =
             m_booster.m_elements.begin();
         it != m_booster.m_elements.end(); ++it)
    {
        int i = 0;
        while (i < it->m_count)
        {
            int rand = get_rand_int(it->m_prob);
            for (std::vector<stStackableBoosterItemInfo_t>::const_iterator it2 =
                     it->m_items.begin();
                 it2 != it->m_items.end(); ++it2)
            {
                int idx = it2->m_itemIdx;
                if (rand < it2->m_prob)
                {
                    out.push_back(*it2);
                    break;
                }
            }
            ++i;
        }
    }
}

bool CStackableItem::getUsableItemType(int type) const
{
    std::vector<int>::const_iterator it =
        std::find(m_usableTypes.begin(), m_usableTypes.end(), type);
    if (it != m_usableTypes.end())
    {
        return 1;
    }
    return 0;
}

int CStackableItem::get_1st_material_count(unsigned int itemIdx)
{
    for (std::vector<std::pair<int, int> >::const_iterator it =
             m_limitCube.m_materials.begin();
         it != m_limitCube.m_materials.end(); it++)
    {
        if ((unsigned int)it->first == itemIdx)
        {
            return it->second;
        }
    }
    return -1;
}

int CStackableItem::get_amplify_reinforce_count(int count)
{
    std::map<int, int>::iterator it = m_amplifyMap.begin();
    std::map<int, int>::iterator end = m_amplifyMap.end();
    int sum = 0;
    int bestKey = 0;
    int bestCount = 0;
    while (it != end)
    {
        sum += it->second;
        if (count < sum)
        {
            return it->first;
        }
        if (bestCount < it->second)
        {
            bestCount = it->second;
            bestKey = it->first;
        }
        ++it;
    }
    return bestKey;
}

void CStackableItem::get_limit_cube_item(std::pair<int, int>& out)
{
    int itemIdx = 0;
    int count = 0;
    int total = 0;
    std::vector<stLimitCubeResult>::const_iterator it =
        m_limitCube.m_results.begin();
    while (it != m_limitCube.m_results.end())
    {
        total += it->m_prob;
        ++it;
    }
    unsigned long range = total;
    int rand = m_pRand->randInt(range);
    int sum = 0;
    std::vector<stLimitCubeResult>::const_iterator it2 =
        m_limitCube.m_results.begin();
    while (it2 != m_limitCube.m_results.end())
    {
        sum += it2->m_prob;
        if (rand <= sum)
        {
            itemIdx = it2->m_itemIdx;
            count = it2->m_count;
            break;
        }
        ++it2;
    }
    out.first = itemIdx;
    out.second = count;
}

bool CStackableItem::is_included_material_for_limit_cube(unsigned int itemIdx)
{
    for (std::vector<std::pair<int, int> >::const_iterator it =
             m_limitCube.m_materials.begin();
         it != m_limitCube.m_materials.end(); it++)
    {
        if ((unsigned int)it->first == itemIdx)
        {
            return 1;
        }
    }
    return 0;
}

int CStackableItem::quickslot_priority()
{
    if (m_nItemType == 0 || m_nItemType == 4)
    {
        return 1;
    }
    return 0;
}

bool CStackableItem::verify()
{
    if (m_usablePeriod != 0 && getStackableLimit() != 1)
    {
        return 0;
    }
    if (m_nItemType == 0xf || m_nItemType == 0x10)
    {
        if (m_booster.m_elements.empty())
        {
            return 0;
        }
    }
    return 1;
}

bool CStackableItem::verifyUsableServerType()
{
    if (m_usableServers.empty())
    {
        return 1;
    }
    int serverType = 0;
    if (((CEnvironmentView*)G_CEnvironment())->m_serverType == 0)
    {
        serverType = 1;
    }
    if (((CEnvironmentView*)G_CEnvironment())->m_serverType == 2)
    {
        serverType = 3;
    }
    if (((CEnvironmentView*)G_CEnvironment())->m_serverType == 1)
    {
        serverType = 2;
    }
    if (std::find(m_usableServers.begin(), m_usableServers.end(), serverType) ==
        m_usableServers.end())
    {
        return 1;
    }
    return 0;
}

// ===================== make_item（ORIG 0x850f8b8） =====================

void CStackableItem::make_item(Inven_Item& item) const
{
    switch (m_nItemType)
    {
    case 8:
        item.m_field1 = 4;
        break;
    case 1:
        item.m_field1 = 3;
        break;
    case 9:
    case 10:
        item.m_field1 = 7;
        break;
    case 0x14:
        item.m_field1 = 9;
        break;
    case 2:
        item.m_field1 = 10;
        break;
    default:
        item.m_field1 = 2;
        break;
    }
    item.m_fieldb = 0;
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
    if (t != 0)
    {
        item.m_fieldb = (unsigned short)((t - 0x44a54a80) / 0x15180);
    }
    item.ResetItemAttr();
    item.m_fieldd = 0;
    ((itemGloballyUniqueIdentifier_t*)((char*)&item + 0x15))->reset();
    item.m_amp.reset();
    if (GetAttachType() == 7)
    {
        item.SetTradeLimitCount((unsigned char)getTradeLimitCount());
    }
}

// ===================== get_lotto_item（ORIG 0x850ebfc） =====================

void CStackableItem::get_lotto_item(char* out)
{
    Inven_Item* pItem = (Inven_Item*)out;
    int itemIdx = -1;
    unsigned long range = 100000;
    int rand = m_pRand->randInt(range);
    int sum = 0;
    __gnu_cxx::__normal_iterator<std::pair<int, int>*,
                                 std::vector<std::pair<int, int> > > it;
    int count = 1;
    std::vector<int>::const_iterator countIt = m_lotto.m_counts.begin();
    std::pair<int, int> cur;
    std::vector<std::pair<int, int> >::iterator pairIt =
        m_lotto.m_items.begin();
    while (pairIt != m_lotto.m_items.end())
    {
        cur = std::pair<int, int>(*pairIt);
        sum += cur.second;
        if (rand < sum)
        {
            itemIdx = cur.first;
            count = *countIt;
            break;
        }
        ++pairIt;
        ++countIt;
    }
    if (itemIdx == 0 && count > 0)
    {
        *(int*)(out + 2) = 0;
        *(int*)(out + 7) = count;
    }
    else if (itemIdx == -1)
    {
        itemIdx = m_lotto.m_defaultIdx;
        count = m_lotto.m_defaultCount;
        if (itemIdx == 0)
        {
            *(int*)(out + 2) = 0;
            *(int*)(out + 7) = count;
            return;
        }
        *(int*)(out + 2) = itemIdx;
        G_CDataManager()->m_itemList->create_item(
            (ENUM_ITEM_CREATE_TYPE)2, *(Inven_Item*)out, count);
    }
    else
    {
        *(int*)(out + 2) = itemIdx;
        G_CDataManager()->m_itemList->create_item(
            (ENUM_ITEM_CREATE_TYPE)2, *(Inven_Item*)out, count);
    }
}

// ===================== verifyUsablePlace（ORIG 0x850e9f4） =====================

bool CStackableItem::verifyUsablePlace(CUser* user)
{
    if (m_usablePlaces.empty())
    {
        return 1;
    }
    int place = 0;
    if (G_GameWorld()->IsPVPChannel())
    {
        place = 3;
    }
    else
    {
        switch (user->get_state())
        {
        case 5:
            place = 2;
            break;
        case 3:
            place = 1;
            break;
        }
    }
    std::vector<StackableAction::UsablePlace>::const_iterator it =
        std::find(m_usablePlaces.begin(), m_usablePlaces.end(), place);
    if (it == m_usablePlaces.end())
    {
        return 0;
    }
    if ((char)m_growType < 0 ||
        ((char)m_growType >= 0 &&
         ((CUserCharacInfo*)user)->getCurCharFirstGrowType() ==
             (char)m_growType))
    {
        return 1;
    }
    return 0;
}

// ===================== set_item（ORIG 0x850d81e） =====================

struct CItemView
{
    char m_pad[0x8c];
    int m_int8c;                 // +0x8c
    char m_pad90[0xc];           // +0x90
    char m_padac[0x24];          // +0xac
    char m_charD0;               // +0xd0
    char m_padD1[0x2b];          // +0xd1
    int m_intFC;                 // +0xfc
    int m_int100;                // +0x100
};

void CStackableItem::set_item(STStackableScript& script)
{
    char buf[500];
    unsigned long seed = (unsigned long)time(0) + (unsigned long)script.m_field0;
    m_pRand = new CMTRand(seed);
    m_index = script.m_field0;
    m_grade = script.m_field4;
    m_field0c = script.m_field14;
    m_field10 = script.m_field18;
    m_field14 = script.m_field1c;
    m_field30 = script.m_field24;
    m_rarity = script.m_field10;
    m_char60 = script.m_fieldfc;
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
        cMyTrace trace("void CStackableItem::set_item(STStackableScript&)",
                       0x80, 5);
        trace("Error!!! %s->%s [%s][%s][%d]", CodePage::script(),
              CodePage::database(), script.m_strac.c_str(),
              "void CStackableItem::set_item(STStackableScript&)", 0x80);
    }
    m_str2c = buf;
    m_usablePeriod = script.m_fieldf4;
    m_expirationDate = script.m_fieldf8;
    m_field64 = script.m_field20;
    m_field68 = script.m_field4ec;
    m_int2cc = script.m_field18c;
    m_pair24.first = script.m_field28;
    m_pair24.second = script.m_field2c;
    m_field50 = script.m_fieldc;
    m_attachType = script.m_field8;
    m_nItemType = script.m_field1e0;
    m_int2b8 = script.m_field3cc;
    if (m_nItemType == 3)
    {
        createRecipeInfo(&m_recipe174, &script.m_vec88, 0);
        m_recipe174.m_vec28 = script.m_vec3c0;
        m_recipe174.additionalInfo(script);
    }
    if (m_nItemType == 6)
    {
        createLotteryInfo(&m_lotto, &script.m_vec88);
    }
    else if (m_nItemType == 7)
    {
        createStackableLotteryInfo(&m_lotto, &script.m_vec88);
    }
    else if (m_nItemType == 0xd)
    {
        createStackableLotteryInfo(&m_lotto, &script.m_vec88);
    }
    else if (m_nItemType == 0x1a || m_nItemType == 0x1b)
    {
        createStackableMultiBoxLotteryInfo(
            &m_multiBox, &script.m_vec244, &script.m_vec250,
            &script.m_vec25c);
    }
    else if (m_nItemType == 0xe || m_nItemType == 0x26)
    {
        m_pkg21c = script.m_vec268;
        m_pkg234 = script.m_vec274;
        if (!script.m_vec2a4.empty())
        {
            m_pkg228 = script.m_vec2a4;
            script.m_vec2a4.clear();
        }
    }
    else if (m_nItemType == 0xf || m_nItemType == 0x10)
    {
        m_booster = script.m_booster;
    }
    else if (m_nItemType == 0x11)
    {
        m_booster = script.m_booster;
        m_nItemType = 0x10;
    }
    else if (m_nItemType == 0x22)
    {
        m_booster = script.m_booster;
        m_boosterSelect = script.m_boosterSelect;
    }
    else if (m_nItemType == 0x17)
    {
        createGlobalEffectInfo(m_vec280, &script.m_vec88);
    }
    else if (m_nItemType == 0x1d)
    {
        m_limitCube.m_materials = script.m_vec3d4;
        m_limitCube.m_field0c = script.m_field3e0;
        m_limitCube.m_field10 = script.m_field3e4;
        m_limitCube.m_results = script.m_vec3e8;
    }
    for (int i = 0; i <= 10; ++i)
    {
        m_jobUsable[i] = script.m_usable[i];
    }
    m_usableLevel = script.m_field5c;
    m_field4c = script.m_field60;
    m_stackable = 1;
    m_int28c = script.m_field364;
    m_int2bc = script.m_field1e4;
    m_usableTypes.clear();
    if (m_nItemType == 2 && m_int2bc == 1)
    {
        if (script.m_vec94.size() > 1)
        {
            for (int i = 1; i < (int)script.m_vec94.size(); ++i)
            {
                int t = GetEquipmentTypeStringToEnum(script.m_vec94[i]);
                if (t != 0x16)
                {
                    m_usableTypes.push_back((int)t);
                }
            }
        }
    }
    m_int2d0 = script.m_field68;
    m_int2d4 = script.m_field1b4;
    m_char2d8 = script.m_field1b9;
    m_char2d9 = script.m_field1ba;
    m_char2da = script.m_field1bb;
    m_char2e0 = script.m_field360;
    m_int2dc = script.m_field358;
    m_char2e1 = script.m_field1bf;
    m_char2e4 = script.m_field1bc;
    m_char2e5 = script.m_field1bd;
    m_char2e6 = script.m_field1be;
    m_char2e7 = script.m_field1c0;
    m_int2e8 = script.m_field1c4;
    m_char2ec = script.m_field1d0;
    m_short2b4 = script.m_field380;
    m_itemGroupName = script.m_field100;
    m_char78 = script.m_field104;
    m_int58 = -1;
    m_bool79 = 0;
    m_int80 = script.m_fieldc8;
    m_int7c = script.m_fieldcc;
    m_impossibleGift = script.m_fieldd0;
    m_strb0 = script.m_str6c;
    m_drop = script.m_specialDrop;
    script.m_specialDrop.clear();
    m_char2f0 = script.m_field10e;
    m_short2ee = script.m_field394;
    m_enchantMap = script.m_map3f4;
    m_char30c = script.m_field40c;
    memcpy(m_chars30d, script.m_fields40d, 2);
    m_char30f = script.m_field396;
    m_short310 = script.m_field398;
    m_char312 = script.m_field39a;
    m_char313 = script.m_field39b;
    m_short314 = (unsigned short)script.m_field39c;
    m_char316 = script.m_field3a0;
    m_char320 = script.m_field438;
    if (m_char320)
    {
        m_int318 = script.m_field43c;
        m_int31c = script.m_field440;
    }
    m_dropBD = script.m_field10f;
    m_vecC0 = script.m_vec110;
    m_questBonus = 0;
    ((CItemView*)this)->m_charD0 = script.m_field3d0;
    m_char321 = script.m_field3d1;
    m_setd4 = script.m_setdc;
    m_intEC = script.m_field64;
    m_intF0 = script.m_field444;
    m_intF4 = script.m_field448;
    ((CItemView*)this)->m_intFC = script.m_field150;
    ((CItemView*)this)->m_int100 = script.m_field154;
    m_vec104.clear();
    m_vec104 = script.m_vec158;
    m_int324 = script.m_field44c;
    m_int328 = script.m_field454;
    m_int32c = script.m_field450;
    m_int330 = script.m_field458;
    m_int334 = script.m_field45c;
    m_int338 = script.m_field460;
    m_int33c = script.m_field464;
    m_set128 = script.m_set11c;
    if (script.m_field474 != 0)
    {
        G_CDataManager()->set_lottery_use_cost(script.m_field0);
    }
    m_int340 = script.m_field478;
    m_char344 = script.m_field3a1;
    m_amplifyMap = script.m_map3a4;
    m_int148 = script.m_field178;
    m_growType = script.m_field17c;
    m_str150 = script.m_str180;
    m_str154 = script.m_str184;
    m_str158 = script.m_str188;
    if (!script.m_vec19c.empty())
    {
        m_vec168.reserve(script.m_vec19c.size());
        std::copy(script.m_vec19c.begin(), script.m_vec19c.end(),
                  std::back_inserter(m_vec168));
    }
    if (!script.m_vec190.empty())
    {
        m_vec15c.reserve(script.m_vec190.size());
        std::copy(script.m_vec190.begin(), script.m_vec190.end(),
                  std::back_inserter(m_vec15c));
    }
    m_int360 = script.m_field418;
    m_char364 = script.m_field32c;
    m_tail365.m_short365 = script.m_field47c;
    m_tail365.m_char367 = script.m_field47e;
    m_set368 = script.m_set480;
    m_set380 = script.m_set498;
    m_avatarOptionChange = script.m_field4b0;
    m_int39c = script.m_field1e8;
    m_vec3a0 = script.m_vec1ec;
    m_usablePlaces = script.m_vec1f8;
    if (m_int39c == 0x2c)
    {
        for (unsigned int i = 0; i < m_vec3a0.size(); ++i)
        {
            void* emo = ((CChattingEmoticonList*)
                ((CDataManagerView*)G_CDataManager())->m_field28)
                ->find_emoticon(m_vec3a0[i]);
            typedef void (*EmoticonFn)(void*, int);
            EmoticonFn fn = *(EmoticonFn*)(*(char**)emo + 0x18);
            fn(emo, m_index);
        }
    }
    m_usableServers = script.m_vec214;
    m_char3c5 = script.m_field205;
    m_impossibleCharacCargo = script.m_fieldda;
    ((CItemView*)this)->m_int8c = script.m_field220;
    m_vec90 = script.m_vec224;
    DyeInfo_assign(m_dyeInfo9c, script.m_dye164);
    m_randomOptionUnseal = script.m_field4b8 != 0;
    m_char144 = script.m_fieldd8;
    m_parentBoosterItemId = 0;
    m_int3d0 = script.m_field468;
    m_int3c8 = script.m_field46c;
    if (m_int3c8 > 1)
    {
        m_booster = script.m_booster;
    }
    m_int3cc = script.m_field1ac;
    m_tradeLimitCount = script.m_field508;
    m_set3d8 = script.m_set50c;
}

// ===================== get_multibox_default_item（ORIG 0x850f136） =====================

void CStackableItem::get_multibox_default_item(
    std::vector<std::pair<Inven_Item, bool> >* out)
{
    if (out == 0)
    {
        return;
    }
    for (unsigned int i = 0;
         i < (unsigned int)m_multiBox.m_defaultCount; ++i)
    {
        int rand = get_rand_int(1000000);
        int sum = 0;
        int itemIdx = 0;
        int count = 0;
        bool flag = false;
        for (std::vector<MultiBoxLotteryInfo::stDefaultItem>::iterator it =
                 m_multiBox.m_items.begin();
             it != m_multiBox.m_items.end(); ++it)
        {
            sum += it->m_prob;
            if (rand < sum)
            {
                itemIdx = it->m_itemIdx;
                count = it->m_count;
                flag = it->m_flag != 0;
                break;
            }
        }
        if (itemIdx == 0)
        {
            itemIdx = m_multiBox.m_defaultIdx;
            count = m_multiBox.m_defaultNum;
            flag = m_multiBox.m_defaultFlag != 0;
        }
        Inven_Item item;
        item.m_addInfo = itemIdx;
        G_CDataManager()->m_itemList->create_item(
            (ENUM_ITEM_CREATE_TYPE)2, item, count);
        std::pair<Inven_Item, bool> p;
        p.first = item;
        p.second = flag;
        out->push_back(p);
    }
}

// ===================== get_multibox_default_item_arad（ORIG 0x850f38a） =====================

void CStackableItem::get_multibox_default_item_arad(
    std::vector<std::pair<Inven_Item, bool> >* out)
{
    if (out == 0)
    {
        return;
    }
    static std::vector<MultiBoxLotteryInfo::LotteryDefaultArad>
        defaultItemListArad;
    if (defaultItemListArad.empty())
    {
        MultiBoxLotteryInfo::LotteryDefaultArad a(0x28be16, 10, 50000);
        defaultItemListArad.push_back(a);
        MultiBoxLotteryInfo::LotteryDefaultArad b(0x28be16, 0x32, 25000);
        defaultItemListArad.push_back(b);
        MultiBoxLotteryInfo::LotteryDefaultArad c(0x28be16, 100, 5000);
        defaultItemListArad.push_back(c);
        MultiBoxLotteryInfo::LotteryDefaultArad d(0x28d150, 1, 10000);
        defaultItemListArad.push_back(d);
    }
    int itemIdx = 0;
    int count = 0;
    int flag = 0;
    int sum = 0;
    unsigned int i = 0;
    if (i < defaultItemListArad.size())
    {
        int rand = get_rand_int(100000);
        sum = 0;
        itemIdx = 0;
        for (std::vector<MultiBoxLotteryInfo::LotteryDefaultArad>::iterator
                 it = defaultItemListArad.begin();
             it != defaultItemListArad.end(); ++it)
        {
            sum += it->m_field8;
            if (rand <= sum)
            {
                itemIdx = it->m_field0;
                count = it->m_field4;
                break;
            }
        }
    }
    if (itemIdx == 0)
    {
        itemIdx = m_multiBox.m_defaultIdx;
        count = m_multiBox.m_defaultNum;
        flag = m_multiBox.m_defaultFlag;
    }
    Inven_Item item;
    item.m_addInfo = itemIdx;
    G_CDataManager()->m_itemList->create_item(
        (ENUM_ITEM_CREATE_TYPE)2, item, count);
    std::pair<Inven_Item, bool> p;
    p.first = item;
    p.second = flag != 0;
    out->push_back(p);
}

// ===================== get_multibox_lottery_item（ORIG 0x850edda） =====================

void CStackableItem::get_multibox_lottery_item(
    std::vector<std::pair<Inven_Item, bool> >* out)
{
    if (out == 0)
    {
        return;
    }
    for (std::vector<MultiBoxLotteryInfo::LotteryUnit>::iterator unitIt =
             m_multiBox.m_units.begin();
         unitIt != m_multiBox.m_units.end(); ++unitIt)
    {
        MultiBoxLotteryInfo::LotteryUnit unit(*unitIt);
        for (int i = 0; i < unit.m_count; ++i)
        {
            int rand = get_rand_int(1000000);
            int sum = 0;
            int itemIdx = 0;
            int count = 0;
            bool flag = false;
            std::vector<std::pair<int, int> >::iterator it =
                unit.m_items.begin();
            std::vector<std::pair<int, int> >::iterator it2 =
                unit.m_bonus.begin();
            while (it != unit.m_items.end())
            {
                sum += it->second;
                if (rand < sum)
                {
                    itemIdx = it->first;
                    count = it2->first;
                    flag = it2->second != 0;
                    break;
                }
                ++it;
                ++it2;
            }
            if (itemIdx == 0)
            {
                itemIdx = unit.m_defaultIdx;
                count = unit.m_defaultNum;
                flag = unit.m_defaultFlag != 0;
            }
            Inven_Item item;
            item.m_addInfo = itemIdx;
            G_CDataManager()->m_itemList->create_item(
                (ENUM_ITEM_CREATE_TYPE)2, item, count);
            std::pair<Inven_Item, bool> p;
            p.first = item;
            p.second = flag;
            out->push_back(p);
        }
    }
}

// ===================== 简单虚函数 / 访问器（ORIG weak） =====================

int CStackableItem::GetItemType() const
{
    return m_nItemType;
}

bool CStackableItem::IsAvatarItem() const
{
    return 0;
}

bool CStackableItem::IsCreatureItem() const
{
    if (m_nItemType > 8 && m_nItemType <= 0xa)
    {
        return 1;
    }
    return 0;
}

bool CStackableItem::IsClearAvatar() const
{
    return 0;
}

bool CStackableItem::IsEnchantalbe() const
{
    return 0;
}

bool CStackableItem::isEnableAvatarFunc(ENUM_AVATAR_FUNC_FILTER filter) const
{
    return 0;
}

int CStackableItem::GetGuildPowerWarPoint() const
{
    return m_int80;
}

int CStackableItem::GetUserPowerWarPoint() const
{
    return m_int7c;
}

bool CStackableItem::IsImpossibleGift() const
{
    return m_impossibleGift;
}

bool CStackableItem::IsRandomOptionUnseal() const
{
    return m_randomOptionUnseal;
}

void CStackableItem::set_parent_booster_item_id(unsigned long id)
{
    m_parentBoosterItemId = id;
}

unsigned long CStackableItem::get_parent_booster_item_id() const
{
    return m_parentBoosterItemId;
}

int CStackableItem::getTradeLimitCount() const
{
    return m_tradeLimitCount;
}
