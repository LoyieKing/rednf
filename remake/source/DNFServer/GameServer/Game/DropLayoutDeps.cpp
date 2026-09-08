// ============================================================================
// DropLayoutDeps.cpp — WongWork 掉落生成器域：生命周期 + 脚本装载（独占 TU）
//
// 全部符号经 df_game_r nm 核实；实现逐条对照 docs/class_func_reports/
// 对应报告的 ## 汇编 + ## 反编译 C（无独立报告者以 objdump 直转并标注）。
// 类布局/声明唯一权威：CItemGenerator.h（本 TU 的姊妹批次扩写）。
//
// 本 TU 独家提供（勿在他处重复定义）：
//   WongWork::CGeneratorCommonData C1/D1/loadScript/getGoldGenTable
//   WongWork::{CMonsterDrop,CObjectDrop,CClearReward,CDungeonDrop} C1 与
//     loadScript（CDungeonDrop ORIG 无 loadScript 覆写符号，不实现）
//   WongWork::CMonsterDrop_Hell / CSpecialMonsterDrop C1/D1/loadScript
//   WongWork::CMonsterDrop::stFirstMobHuntingWeight_t C1 → 归 WongWorkDrop_A.cpp
//   WongWork::{CItemGenRateTable,CSecialIMonsterDropItemGenRateTable}
//     C1/~D1/init/destroy/generateTable/_itemTypeVerify/
//     set_disable_drop_random_option_item(仅前者)
//   WorldDropInfo C1/D1（objdump 0x08374ec8 / 0x08374ede 直转）
//   random_option::CRandomOptionItemHandle::set_disable_drop（W 0x08539e42；
//     C1/D1 复用 GlobalData.cpp 权威定义）
//   CItem::{get_gen_rate,isSpecialMonsterDropItem,isDropItemDungeonDifficulty,
//     isDropItemDungeonType}、CItemList::getItemGradeList
//   CDataManager::GetAutoMarketContitionsControl（W 0x084e92fc）
//   CAutoMarketConditionsControl::GetGoldDropAmountRate（T 0x082f89bc）
// ============================================================================

#include "GameTypes.h"       // cMyTrace（GetGoldDropAmountRate 千次日志路径）
#include <cstring>
#include <ctime>

#include "CItemGenerator.h"   // 域内全部类声明（含生成率表/Hell/Special）
#include "DNFLexWrapper.h"    // loadRDARScriptFile/ScanType/ScanInt/ScanFloat
#include "CDataManager.h"     // G_CDataManager/m_itemList/CItem/CItemList 完整类型

namespace WongWork
{

// stFirstMobHuntingWeight_t（现 WongWork::CMonsterDrop 嵌套类型）C1 已迁移至
// WongWorkDrop_A.cpp（嵌套限定名；此处旧顶层定义删除以免 ODR/符号分裂）。

// ============================================================================
// CItemGenRateTable
// ============================================================================

CItemGenRateTable::CItemGenRateTable()
    : m_rand(0)
{
    // ORIG 0x08539e66：map 循环构造（成员自动）→ m_rand=0 → Handle 构造（自动）→ init()。
    init();
}

CItemGenRateTable::~CItemGenRateTable()
{
    // ORIG 0x0853a030：destroy() → Handle D1 → map D1（后两者成员自动）。
    destroy();
}

void CItemGenRateTable::init()
{
    // ORIG 0x085345b6：旧表非空则 operator delete；time(0) 播种 new 0x9c8（单棵）。
    // ORIG lea -0x1c(%ebp),%esi 跨 new（esi 挂 seed 地址）。4.4 恒
    // `mov %esi,0x4(%esp)` 形不可造（lea esi / -0x1c 槽缓存两形探针全败
    // 26/25），N18a 族封顶。
    if (m_rand != 0)
        delete m_rand;
    unsigned long seed = time(0);
    m_rand = new CMTRand(seed);
}

void CItemGenRateTable::destroy()
{
    // ORIG 0x08534638。
    if (m_rand != 0)
    {
        delete m_rand;
        m_rand = 0;
    }
}

void CItemGenRateTable::set_disable_drop_random_option_item()
{
    // ORIG 0x08539e4e：转发 +0x23554 句柄 set_disable_drop。
    m_optHandle.set_disable_drop();
}

bool CItemGenRateTable::_itemTypeVerify(int type, const CItem* item)
{
    // 报告 _itemTypeVerify.md 逐分支转写（T 0x08534906）。
    if (item->is_stackable())
    {
        if (item->GetItemType() == 3 && type == 3)
            return true;
        if (type == 1)
            return true;
    }
    else
    {
        if (item->IsCreatureItem() && type == 4)
            return true;
        if (type == 2)
            return true;
    }
    return false;
}

void CItemGenRateTable::generateTable(stGenItemRange_t* range, bool keepRandomOption)
{
    // 报告 generateTable.md（T 0x0853466c）逐循环转写。槽址 =
    // type*0x7110+rarity*0x12d8+level*24 ⇒ m_slots[type][rarity][level]。
    __gnu_cxx::hash_map<int, CItem*>* grades =
        G_CDataManager()->m_itemList->getItemGradeList();
    for (int type = 1; type <= 4; ++type)
    {
        for (int rarity = 0; rarity <= 5; ++rarity)
        {
            for (int level = 1; level <= 200; ++level)
            {
                unsigned int cumulative = 0;
                const unsigned char* pairBytes =
                    reinterpret_cast<const unsigned char*>(&range->m_range[level]);
                int offset = -(int)pairBytes[0];      // -(u8)min
                const int maxOffset = (int)pairBytes[1];
                for (; offset < maxOffset; ++offset)
                {
                    int target = level + offset;
                    if (target > 200)                 // cmp $0xc8 jg → 终止整行
                        break;
                    const __gnu_cxx::hash_map<int, CItem*>& bucket = grades[target];
                    for (__gnu_cxx::hash_map<int, CItem*>::const_iterator
                             it(bucket.begin()), e(bucket.end()); it != e; ++it)
                    {
                        CItem* item = it->second;
                        if (item->get_gen_rate() == 0)
                            continue;
                        if (item->get_rarity() != rarity)
                            continue;
                        if (!_itemTypeVerify(type, item))
                            continue;
                        if (item->isSpecialMonsterDropItem())
                            continue;
                        // param_2 假时剔除随机选项物品（虚槽 0x4c =
                        // CItem::IsRandomOption；df_game_r vtable 0x8c96da0+0x4c 实证）。
                        if (!keepRandomOption && item->IsRandomOption())
                            continue;
                        cumulative += (unsigned int)item->get_gen_rate();
                        m_slots[type][rarity][target].insert(
                            std::make_pair(cumulative, item->get_index()));
                    }
                }
            }
        }
    }
}

// ============================================================================
// CSecialIMonsterDropItemGenRateTable
// ============================================================================

CSecialIMonsterDropItemGenRateTable::CSecialIMonsterDropItemGenRateTable()
    : m_rand(0)
{
    // ORIG 0x0853a0b6：map 循环 → rand=0(+0x211fb0) → Handle(+0x211fb4) → init()。
    init();
}

CSecialIMonsterDropItemGenRateTable::~CSecialIMonsterDropItemGenRateTable()
{
    // ORIG 0x0853a35c。
    destroy();
}

void CSecialIMonsterDropItemGenRateTable::init()
{
    // ORIG 0x08534c1a：与 CItemGenRateTable::init 同款。
    if (m_rand != 0)
        delete m_rand;
    unsigned long seed = time(0);
    m_rand = new CMTRand(seed);
}

void CSecialIMonsterDropItemGenRateTable::destroy()
{
    if (m_rand != 0)
    {
        delete m_rand;
        m_rand = 0;
    }
}

bool CSecialIMonsterDropItemGenRateTable::_itemTypeVerify(int type, const CItem* item)
{
    // 报告 WongWork__CSecialIMonsterDropItemGenRateTable/_itemTypeVerify.md：
    // 与 CItemGenRateTable 版逐分支一致。
    if (item->is_stackable())
    {
        if (item->GetItemType() == 3 && type == 3)
            return true;
        if (type == 1)
            return true;
    }
    else
    {
        if (item->IsCreatureItem() && type == 4)
            return true;
        if (type == 2)
            return true;
    }
    return false;
}

void CSecialIMonsterDropItemGenRateTable::generateTable(stGenItemRange_t* range)
{
    // 报告 generateTable.md（T 0x08534cd0）：五重循环 a(0..4) b(0..2) type(1..4)
    // rarity(0..5) level(1..200)；槽址 a*0x69ff0+b*0x23550+type*0x7110+
    // rarity*0x12d8+level*24。过滤要求 isSpecialMonsterDropItem /
    // isDropItemDungeonDifficulty(a) / isDropItemDungeonType(b) 全真，无随机选项分支。
    __gnu_cxx::hash_map<int, CItem*>* grades =
        G_CDataManager()->m_itemList->getItemGradeList();
    for (int a = 0; a <= 4; ++a)
    {
        for (int b = 0; b <= 2; ++b)
        {
            for (int type = 1; type <= 4; ++type)
            {
                for (int rarity = 0; rarity <= 5; ++rarity)
                {
                    for (int level = 1; level <= 200; ++level)
                    {
                        unsigned int cumulative = 0;
                        const unsigned char* pairBytes = reinterpret_cast<const unsigned char*>(
                            &range->m_range[level]);
                        int offset = -(int)pairBytes[0];
                        const int maxOffset = (int)pairBytes[1];
                        for (; offset < maxOffset; ++offset)
                        {
                            int target = level + offset;
                            if (target > 200)
                                break;
                            __gnu_cxx::hash_map<int, CItem*>& bucket = grades[target];
                            for (__gnu_cxx::hash_map<int, CItem*>::iterator it =
                                     bucket.begin();
                                 it != bucket.end(); ++it)
                            {
                                CItem* item = it->second;
                                if (item->get_gen_rate() == 0)
                                    continue;
                                if (item->get_rarity() != rarity)
                                    continue;
                                if (!_itemTypeVerify(type, item))
                                    continue;
                                if (!item->isSpecialMonsterDropItem())
                                    continue;
                                if (!item->isDropItemDungeonDifficulty(a))
                                    continue;
                                if (!item->isDropItemDungeonType(b))
                                    continue;
                                cumulative += (unsigned int)item->get_gen_rate();
                                m_blocks[a][b][type][rarity][target].insert(
                                    std::make_pair(cumulative, item->get_index()));
                            }
                        }
                    }
                }
            }
        }
    }
}

// ============================================================================
// CMonsterDrop_Hell —— ctor W 0x0853a56a / D1 W 0x0853a60e / loadScript T 0x08535344
// ============================================================================

CMonsterDrop_Hell::CMonsterDrop_Hell(int count)
    : CItemGenerator(count)
{
    // vptr 0x08c9a778 由编译器写入；两向量与 +0x254 表为成员自动构造。
}

CMonsterDrop_Hell::~CMonsterDrop_Hell()
{
}

int CMonsterDrop_Hell::loadScript(char const* path)
{
    if (!loadRDARScriptFile("", path))
        return 0x302;
    std::string token;
    bool ok = false;
    while (ScanType(token, true))
    {
        if (token == "[basis of rarity dicision]")
        {
            int rows = ScanInt(&ok);
            if (!ok)
                return 0x311;
            for (int i = 1; i <= rows; ++i)
            {
                for (int j = 0; j <= 5; ++j)
                {
                    m_rarityBasis[i][j] = ScanInt(&ok);
                    if (!ok)
                        return 0x319;
                }
            }
        }
        else if (token == "[drop prob count]")
        {
            stGenItemProb_t fill;
            memset(&fill, 0, sizeof(fill));
            int count = ScanInt(&ok);
            m_probs.resize(count, fill);
            if (!ok)
                return 0x320;
        }
        else if (token == "[dungeon difficulty drop prob]")
        {
            for (size_t j = 0; j < m_probs.size(); ++j)
            {
                int* e = &m_probs[j].v[0];
                e[0] = ScanInt(&ok);
                if (!ok)
                    return 0x328;
                e[1] = ScanInt(&ok);
                if (!ok)
                    return 0x32a;
                for (int k = 0; k <= 4; ++k)
                {
                    e[2 + k] = ScanInt(&ok);
                    if (!ok)
                        return 0x332;
                }
            }
        }
        else if (token == "[item drop ref table]")
        {
            m_range.m_range[0] = 0;
            for (int lvl = 1; lvl <= 200; ++lvl)
            {
                int idx = ScanInt(&ok);
                if (!ok)
                    return 0x33f;
                unsigned char* entry =
                    reinterpret_cast<unsigned char*>(&m_range.m_range[idx]);
                entry[0] = (unsigned char)ScanInt(&ok);
                if (!ok)
                    return 0x343;
                entry[1] = (unsigned char)ScanInt(&ok);
                if (!ok)
                    return 0x347;
            }
        }
        else
        {
            return 0x34b;
        }
    }
    m_table.generateTable(&m_range, true);
    return 0;
}

// ============================================================================
// CSpecialMonsterDrop —— ctor W 0x0853a426 / D1 W 0x0853a4ca / loadScript T 0x085373ee
// ============================================================================

CSpecialMonsterDrop::CSpecialMonsterDrop(int count)
    : CItemGenerator(count)
{
    // vptr 0x08c9a760；向量 ×2 与 +0x340 SecialIM 表为成员自动构造。
}

CSpecialMonsterDrop::~CSpecialMonsterDrop()
{
}

int CSpecialMonsterDrop::loadScript(char const* path)
{
    if (!loadRDARScriptFile("", path))
        return 0x5fb;
    std::string token;
    bool ok = false;
    while (ScanType(token, true))
    {
        if (token == "[drop prob count]")
        {
            stGenItemProb_t fill;
            memset(&fill, 0, sizeof(fill));
            int count = ScanInt(&ok);
            m_probs.resize(count, fill);
            if (!ok)
                return 0x609;
        }
        else if (token == "[drop prob]")
        {
            for (size_t j = 0; j < m_probs.size(); ++j)
            {
                int* e = &m_probs[j].v[0];
                e[0] = ScanInt(&ok);
                if (!ok)
                    return 0x611;
                e[1] = ScanInt(&ok);
                if (!ok)
                    return 0x613;
                for (int k = 0; k <= 4; ++k)
                {
                    e[2 + k] = ScanInt(&ok);
                    if (!ok)
                        return 0x617;
                }
            }
        }
        else if (token == "[basis of rarity dicision]")
        {
            for (int i = 1; i <= 4; ++i)
            {
                for (int j = 0; j <= 5; ++j)
                {
                    m_rarityBasis[i][j] = ScanInt(&ok);
                    if (!ok)
                        return 0x623;
                }
            }
        }
        else if (token == "[party member drop bonusrate]")
        {
            for (int i = 0; i <= 4; ++i)
            {
                for (int j = 0; j <= 3; ++j)
                {
                    m_partyBonus[i][j] = ScanFloat(&ok);
                    if (!ok)
                        return 0x62e;
                }
            }
        }
        else if (token == "[dungeon difficulty drop bonusrate]")
        {
            for (int b = 0; b <= 4; ++b)
            {
                for (int j = 0; j <= 4; ++j)
                {
                    m_dungeonBonus[b][j] = ScanFloat(&ok);
                    if (!ok)
                        return 0x63d;
                }
            }
        }
        else if (token == "[monster type drop bonusrate]")
        {
            for (int t = 0; t <= 4; ++t)
            {
                for (int j = 0; j <= 3; ++j)
                {
                    m_monsterTypeBonus[t][j] = ScanFloat(&ok);
                    if (!ok)
                        return 0x648;
                }
            }
        }
        else if (token == "[item drop ref table]")
        {
            m_range.m_range[0] = 0;
            for (int lvl = 1; lvl <= 200; ++lvl)
            {
                int idx = ScanInt(&ok);
                if (!ok)
                    return 0x655;
                unsigned char* entry =
                    reinterpret_cast<unsigned char*>(&m_range.m_range[idx]);
                entry[0] = (unsigned char)ScanInt(&ok);
                if (!ok)
                    return 0x659;
                entry[1] = (unsigned char)ScanInt(&ok);
                if (!ok)
                    return 0x65d;
            }
        }
        else
        {
            return 0x661;
        }
    }
    m_table.generateTable(&m_range);
    return 0;
}

// ============================================================================
// CMonsterDrop —— ctor W 0x0853a6c8 / loadScript T 0x08537938 / ×3 T 0x085380fa
// ============================================================================

CMonsterDrop::CMonsterDrop(int count)
    : CItemGenerator(count), m_hell(2), m_special(3)   // ORIG HellC1(2)/SpecialC1(3)
{
    // [B24D判词 N] 语义正确：基类 CItemGenerator(count) + m_hell(2)/m_special(3)
    //   成员 init + m_firstMobWeight=1/m_firstMobWeightRate=1.0f。P167 差集 ORIG
    //   CItemGeneratorC1/D1（完整构造/析构）vs 我方 C2/D2+CSpecialMonsterDropD1
    //   （基类/成员子对象构造析构 mangling），C1/C2、D1/D2 配对同语义，非缺口。
    // ORIG 0x0853a6c8：基类 → vptr 0x08c9a748 → 向量×2/表/Hell/Special/FirstMob
    // （成员自动）→ +0x259380=1 → +0x259384=1.0f。
    m_firstMobWeight = 1;
    m_firstMobWeightRate = 1.0f;
}

int CMonsterDrop::loadScript(char const* path)
{
    if (!loadRDARScriptFile("", path))
        return 0x66b;
    std::string token;
    bool ok = false;
    while (ScanType(token, true))
    {
        if (token == "[drop prob count]")
        {
            stGenItemProb_t fill;
            memset(&fill, 0, sizeof(fill));
            int count = ScanInt(&ok);
            m_probs.resize(count, fill);
            if (!ok)
                return 0x679;
        }
        else if (token == "[drop prob]")
        {
            for (size_t j = 0; j < m_probs.size(); ++j)
            {
                int* e = &m_probs[j].v[0];
                e[0] = ScanInt(&ok);
                if (!ok)
                    return 0x681;
                e[1] = ScanInt(&ok);
                if (!ok)
                    return 0x683;
                for (int k = 0; k <= 4; ++k)
                {
                    e[2 + k] = ScanInt(&ok);
                    if (!ok)
                        return 0x687;
                }
            }
        }
        else if (token == "[basis of rarity dicision]")
        {
            for (int i = 1; i <= 4; ++i)
            {
                for (int j = 0; j <= 5; ++j)
                {
                    m_rarityBasis[i][j] = ScanInt(&ok);
                    if (!ok)
                        return 0x693;
                }
            }
        }
        else if (token == "[party member drop bonusrate]")
        {
            for (int i = 0; i <= 4; ++i)
            {
                for (int j = 0; j <= 3; ++j)
                {
                    m_partyBonus[i][j] = ScanFloat(&ok);
                    if (!ok)
                        return 0x69e;
                }
            }
        }
        else if (token == "[dungeon difficulty drop bonusrate]")
        {
            for (int b = 0; b <= 4; ++b)
            {
                for (int j = 0; j <= 4; ++j)
                {
                    m_dungeonBonus[b][j] = ScanFloat(&ok);
                    if (!ok)
                        return 0x6ad;
                }
            }
        }
        else if (token == "[monster type drop bonusrate]")
        {
            for (int t = 0; t <= 4; ++t)
            {
                for (int j = 0; j <= 3; ++j)
                {
                    m_monsterTypeBonus[t][j] = ScanFloat(&ok);
                    if (!ok)
                        return 0x6b8;
                }
            }
        }
        else if (token == "[item drop ref table]")
        {
            m_range.m_range[0] = 0;
            for (int lvl = 1; lvl <= 200; ++lvl)
            {
                int idx = ScanInt(&ok);
                if (!ok)
                    return 0x6c5;
                unsigned char* entry =
                    reinterpret_cast<unsigned char*>(&m_range.m_range[idx]);
                entry[0] = (unsigned char)ScanInt(&ok);
                if (!ok)
                    return 0x6c9;
                entry[1] = (unsigned char)ScanInt(&ok);
                if (!ok)
                    return 0x6cd;
            }
        }
        else if (token == "[first boss/named mob hunting]")
        {
            // ORIG：先 [+0x25937c]=1 拷到 [+0x259378]，再两次扫描覆写。
            m_firstMobHunt.m_field04 = 1;
            m_firstMobHunt.m_weight = m_firstMobHunt.m_field04;
            m_firstMobHunt.m_weight = ScanInt(&ok);
            if (!ok)
                return 0x6d7;
            m_firstMobHunt.m_field04 = ScanInt(&ok);
            if (!ok)
                return 0x6db;
        }
        else if (token == "[condition rate]")
        {
            for (int k = 0; k <= 3; ++k)
            {
                m_conditionRate[k].rate = (unsigned short)ScanInt(&ok);
                if (!ok)
                    return 0x6e5;
                m_conditionRate[k].value = (unsigned short)ScanInt(&ok);
                if (!ok)
                    return 0x6e9;
            }
        }
        else if (token == "[gold quantity]")
        {
            for (int k = 0; k <= 3; ++k)
            {
                m_goldQuantity[k] = (unsigned short)ScanInt(&ok);
                if (!ok)
                    return (k == 0) ? 0x6f0 : (k == 1) ? 0x6f4 : (k == 2) ? 0x6f8 : 0x6fc;
            }
        }
        else if (token == "[gold volume]")
        {
            m_goldVolume[0] = (unsigned short)ScanInt(&ok);
            if (!ok)
                return 0x702;
            m_goldVolume[1] = (unsigned short)ScanInt(&ok);
            if (!ok)
                return 0x706;
        }
        else
        {
            return 0x70a;
        }
    }
    m_table.generateTable(&m_range, true);
    return 0;
}

int CMonsterDrop::loadScript(char const* path, char const* hell, char const* special)
{
    // ORIG T 0x085380fa：虚槽调单参版 → +0x238ac Hell → 第三参非空才装 Special。
    if (loadScript(path) != 0)
        return 0x715;
    int err = 0;                                  // ORIG movl $0,-0xc 预清
    err = m_hell.loadScript(hell);
    if (err != 0)
        return err;
    if (special != 0)
    {
        err = m_special.loadScript(special);
        if (err != 0)
            return err;
    }
    return 0;
}

// ============================================================================
// CObjectDrop —— ctor W 0x0853a7f8 / loadScript T 0x08538352
// ============================================================================

CObjectDrop::CObjectDrop(int count)
    : CMonsterDrop(count)
{
    // ORIG：CMonsterDrop C1 → vptr 0x08c9a730（编译器自动）。
}

int CObjectDrop::loadScript(char const* path)
{
    // ORIG T 0x08538352：先把队伍加成区 (+0xa8..0xf7) 全写 1.0f，再非虚直呼
    // CMonsterDrop::loadScript（0x08537938）。
    for (int i = 0; i <= 4; ++i)
        for (int j = 0; j <= 3; ++j)
            m_partyBonus[i][j] = 1.0f;
    return CMonsterDrop::loadScript(path);
}

// ============================================================================
// CClearReward —— ctor W 0x0853a968 / loadScript T 0x085392c0
// ============================================================================

CClearReward::CClearReward(int count)
    : CItemGenerator(count)
{
    // ORIG 0x0853a968：基类 → vptr 0x08c9a700 → 7×vector(+0x18 步进 0xc，第 7 个在
    // +0x60) → 表 ctor(+0x2a8) → 两个空白卡 vector(+0x23b40/+0x23b78) →
    // 金卡费用表 [201] 清零 → +0x23b38=0.0f → +0x23b3c=0 → +0x23b74=0 →
    // memset(+0x23b4c,0,0x28) → set_disable_drop_random_option_item()。
    // 成员对象由编译器按声明序完成；标量/清零在此补齐。
    memset(m_goldCardCost, 0, sizeof(m_goldCardCost));
    m_goldCardCreateRate = 0.0f;
    m_goldCardBlankTotal = 0;
    m_pcRoomBlankTotal = 0;
    memset(m_rewardRatePerMap, 0, sizeof(m_rewardRatePerMap));
    m_table.set_disable_drop_random_option_item();
}

int CClearReward::loadScript(char const* path)
{
    if (!loadRDARScriptFile("", path))
        return 0x943;
    std::string section;
    std::string token;
    bool ok = false;
    while (ScanType(section, true))
    {
        if (section == "[drop prob count]")
        {
            int count = ScanInt(&ok);
            if (!ok)
                return 0x951;
            stGenItemProb_t fill;
            fill.v[0] = fill.v[1] = fill.v[2] = 0;
            for (int i = 0; i <= 5; ++i)
                m_scriptProbs[i].resize(count, fill);
        }
        else if (section == "[drop prob]")
        {
            // ScanStr 选槽：default/event/event2/pcroom default/pcroom event/
            // pcroom bonus → 0..5；未知 → 0x969。扫描失败静默退出本节。
            for (;;)
            {
                if (!ScanStr(&token))
                    break;
                int slot;
                if (token == "default")
                    slot = 0;
                else if (token == "event")
                    slot = 1;
                else if (token == "event2")
                    slot = 2;
                else if (token == "pcroom default")
                    slot = 3;
                else if (token == "pcroom event")
                    slot = 4;
                else if (token == "pcroom bonus")
                    slot = 5;
                else
                    return 0x969;
                for (size_t j = 0; j < m_scriptProbs[slot].size(); ++j)
                {
                    int* e = &m_scriptProbs[slot][j].v[0];
                    e[0] = ScanInt(&ok);
                    if (!ok)
                        return 0x96f;
                    e[1] = ScanInt(&ok);
                    if (!ok)
                        return 0x971;
                    e[2] = ScanInt(&ok);
                    if (!ok)
                        return 0x973;
                }
            }
        }
        else if (section == "[drop kind prob]")
        {
            m_dropKindProb[0] = ScanInt(&ok);
            if (!ok)
                return 0x97a;
            m_dropKindProb[1] = ScanInt(&ok);
            if (!ok)
                return 0x97c;
        }
        else if (section == "[drop item type prob]")
        {
            for (int i = 1; i <= 4; ++i)
            {
                m_dropItemTypeProb[i - 1] = ScanInt(&ok);   // ORIG 索引 1..4 → 下标 i-1
                if (!ok)
                    return 0x984;
            }
        }
        else if (section == "[basis of rarity dicision]")
        {
            for (int j = 0; j <= 5; ++j)
            {
                m_rarityBasis[j] = ScanInt(&ok);
                if (!ok)
                    return 0x98d;
            }
        }
        else if (section == "[dungeon difficulty drop bonusrate]")
        {
            for (int b = 0; b <= 4; ++b)
            {
                m_dungeonDropBonus[b] = ScanInt(&ok);
                if (!ok)
                    return 0x99a;
            }
        }
        else if (section == "[party member drop bonusrate]")
        {
            for (int f = 0; f <= 3; ++f)
            {
                m_partyBonus[f] = ScanFloat(&ok);
                if (!ok)
                    return 0x9a3;
            }
        }
        else if (section == "[dungeon difficulty gold drop bonusrate]")
        {
            for (int g = 0; g <= 4; ++g)
            {
                m_dungeonGoldBonus[g] = ScanFloat(&ok);
                if (!ok)
                    return 0x9b0;
            }
        }
        else if (section == "[item drop ref table]")
        {
            m_range.m_range[0] = 0;
            for (int lvl = 1; lvl <= 200; ++lvl)
            {
                int idx = ScanInt(&ok);
                if (!ok)
                    return 0x9bc;
                unsigned char* entry =
                    reinterpret_cast<unsigned char*>(&m_range.m_range[idx]);
                entry[0] = (unsigned char)ScanInt(&ok);
                if (!ok)
                    return 0x9c0;
                entry[1] = (unsigned char)ScanInt(&ok);
                if (!ok)
                    return 0x9c4;
            }
        }
        else if (section == "[gold card cost table]")
        {
            // 扫描失败静默结束本节（ORIG 跳回节分发）。
            for (;;)
            {
                int idx = ScanInt(&ok);
                if (!ok)
                    break;
                m_goldCardCost[idx] = ScanInt(&ok);
                if (!ok)
                    return 0x9d1;
            }
        }
        else if (section == "[gold card create rate]")
        {
            m_goldCardCreateRate = ScanFloat(&ok);
            if (!ok)
                return 0x9d8;
        }
        else if (section == "[pcroom card blank item]")
        {
            m_pcRoomBlankTotal = 0;
            for (;;)
            {
                stGoldCardBlankItemInfo_t info;
                info.v[0] = ScanInt(&ok);
                if (!ok)
                    break;
                info.v[1] = ScanInt(&ok);
                if (!ok)
                    return 0x9e6;
                info.v[2] = ScanInt(&ok);
                if (!ok)
                    return 0x9e9;
                m_pcRoomBlanks.push_back(info);
                m_pcRoomBlankTotal += info.v[0];   // ORIG：count += 首值
            }
        }
        else if (section == "[reward item rate per map max count]")
        {
            for (;;)
            {
                int idx = ScanInt(&ok);
                if (!ok)
                    break;
                if (idx > 9)
                    return 0x9f6;
                m_rewardRatePerMap[idx] = ScanInt(&ok);
                if (!ok)
                    return 0x9f9;
            }
        }
        else if (section == "[gold card blank item]")
        {
            m_goldCardBlankTotal = 0;
            for (;;)
            {
                stGoldCardBlankItemInfo_t info;
                info.v[0] = ScanInt(&ok);
                if (!ok)
                    break;
                info.v[1] = ScanInt(&ok);
                if (!ok)
                    return 0xa08;
                info.v[2] = ScanInt(&ok);
                if (!ok)
                    return 0xa0b;
                m_goldBlanks.push_back(info);
                m_goldCardBlankTotal += info.v[0];
            }
        }
        else
        {
            return 0xa41;
        }
    }
    m_table.generateTable(&m_range, false);
    return 0;
}

// ============================================================================
// CDungeonDrop —— ctor W 0x0853abc2（ORIG 无 loadScript 覆写符号）
// ============================================================================

CDungeonDrop::CDungeonDrop(int count)
    : CItemGenerator(count)
{
    // vptr 0x08c9a6e8 由编译器写入；无其余成员。
}

// ============================================================================
// CGeneratorCommonData —— C1/D2 W 0x08374ebc/ec2（空体）/loadScript T 0x08534456 /
// getGoldGenTable T 0x08533424
// ============================================================================

CGeneratorCommonData::CGeneratorCommonData() {}

CGeneratorCommonData::~CGeneratorCommonData() {}

int CGeneratorCommonData::loadScript(const char* path)
{
    if (!loadRDARScriptFile("", path))
        return 0x1d8;
    std::string token;
    bool ok = false;
    while (ScanType(token, true))
    {
        if (token != "[gold drop ref table]")
            return 0x1f8;
        m_goldGenTable[0].m_base = 0;
        m_goldGenTable[0].m_randAmp = 0;
        for (int i = 1; i <= 200; ++i)
        {
            int idx = ScanInt(&ok);          // 写入下标来自脚本首列（ORIG 同形）
            if (!ok)
                return 0x1ec;
            m_goldGenTable[idx].m_base = ScanInt(&ok);
            if (!ok)
                return 0x1f0;
            m_goldGenTable[idx].m_randAmp = ScanInt(&ok);
            if (!ok)
                return 500;                  // 0x1f4
        }
    }
    return 0;
}

int* CGeneratorCommonData::getGoldGenTable(unsigned char level)
{
    // ORIG T 0x08533424：静态返回区（bss 0x943ded0，函数局部 static 名 ret）；
    // level>0xc8 回退表项 0 并按 0 级取费率；末次经 AutoMarket 折算写回首字。
    static GoldGenEntry ret;
    if (level > 0xc8)
    {
        ret = m_goldGenTable[0];
        level = 0;
    }
    else
    {
        ret = m_goldGenTable[level];
    }
    ret.m_base = G_CDataManager()->GetAutoMarketContitionsControl()->GetGoldDropAmountRate(
        (int)level, ret.m_base);
    return &ret.m_base;
}

}  // namespace WongWork

// ============================================================================
// WorldDropInfo 生命周期（本 TU 唯一 owner；无独立报告，objdump 直转）：
//   C1 0x08374ec8：仅构造 +0x04 的 vector<WorldDropItemChunck>；
//   D1 0x08374ede：仅析构该 vector；标量成员 m_first/m_total 无动作。
// ============================================================================
WorldDropInfo::WorldDropInfo() : m_chunks() {}
WorldDropInfo::~WorldDropInfo() {}

// ============================================================================
// random_option::CRandomOptionItemHandle::set_disable_drop —— W 0x08539e42
// （objdump：movb $0x1,0xc(%eax)。C1/D1 权威定义在 GlobalData.cpp。）
// ============================================================================
namespace random_option
{
void CRandomOptionItemHandle::set_disable_drop()
{
    m_disabled = 1;
}
}  // namespace random_option

// ============================================================================
// CItem 访问器族（W inline；字段名见 CItem.h/STItemScript.h）
// ============================================================================
int CItem::get_gen_rate() const
{
    return m_field50;                       // ORIG 0x832e02a：mov 0x50(%eax),%eax
}

unsigned char CItem::isSpecialMonsterDropItem()
{
    return m_drop.m_enable;                 // ORIG 0x8539e00：movzbl 0xb4(%eax)
}

unsigned char CItem::isDropItemDungeonDifficulty(int idx)
{
    return m_drop.m_difficulty[idx];        // ORIG 0x8539e10：movzbl 0xb5(%edx,%eax)
}

unsigned char CItem::isDropItemDungeonType(int idx)
{
    return m_drop.m_dungeonType[idx];       // ORIG 0x8539e24：movzbl 0xba(%edx,%eax)
}

// ============================================================================
// CItemList::getItemGradeList —— W 0x8539e38（返回 +0x00 的等级分桶表指针）
// ============================================================================
__gnu_cxx::hash_map<int, CItem*, __gnu_cxx::hash<int>, std::equal_to<int>,
                    std::allocator<CItem*> >* CItemList::getItemGradeList() const
{
    return m_gradeItemList;
}

// ============================================================================
// CDataManager::GetAutoMarketContitionsControl —— ORIG W 0x084e92fc
// （objdump：mov 0x8cc4(%eax),%eax —— 即成员 m_autoMarket）
// ============================================================================
CAutoMarketConditionsControl* CDataManager::GetAutoMarketContitionsControl()
{
    return m_autoMarket;
}

// ============================================================================
// CAutoMarketConditionsControl::GetGoldDropAmountRate —— 报告 GetGoldDropAmountRate.md
// （ORIG T 0x082f89bc 逐分支转写；除数 -1000.0 为 .rodata 0x8c1fc70 双字实测）
// ============================================================================
int CAutoMarketConditionsControl::GetGoldDropAmountRate(int level, int gold)
{
    int result = 0;                         // ORIG prolog 后立即清零（-0x14）
    double rate;                            // ORIG 不预初始化（仅 else 路径赋值）
    if (m_field00 == 0)
        result = gold;                      // 未启用折算：原样返回（ORIG 尾部共用块 result=gold）
    else
    {
        // rate = cond[level]*(1-base)/(-1000) + 1；出 (0,1] 则回退 base。
        rate = ((double)m_goldPerLevel[level] * (1.0 - m_goldBaseRate)) / -1000.0 + 1.0;
        if (!(rate > 0.0) || rate > 1.0)
            rate = m_goldBaseRate;
        result = (int)((double)gold * rate);         // ORIG fistpl 直接截断（无 +0.5）
    }
    static int logCount = 0;                // ORIG bss _ZZ...GetGoldDropAmountRateEiiE8logCount
    bool logIt = (logCount % 1000 == 0);
    ++logCount;
    if (logIt)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__, 0xce, 0);
        trace("goldDropAmountRate(%lf) gold(%d), baseGold(%d)", rate, result, gold);
    }
    return result;
}

// 布局自检（嵌入尺寸三重互证见文件头）
// ============================================================================
typedef char assert_drop_range[sizeof(WongWork::stGenItemRange_t) == 0x194 ? 1 : -1];
typedef char assert_rate_table[sizeof(WongWork::CItemGenRateTable) == 0x2356C ? 1 : -1];
typedef char assert_secial_table[sizeof(WongWork::CSecialIMonsterDropItemGenRateTable) ==
                                 0x211FCC ? 1 : -1];
typedef char assert_firstmob[sizeof(WongWork::stFirstMobHuntingWeight_t) == 8 ? 1 : -1];
typedef char assert_opt_handle[sizeof(random_option::CRandomOptionItemHandle) == 0x18 ? 1 : -1];
typedef char assert_hell[sizeof(WongWork::CMonsterDrop_Hell) == 0x237C0 ? 1 : -1];
typedef char assert_special[sizeof(WongWork::CSpecialMonsterDrop) == 0x21230C ? 1 : -1];
typedef char assert_monster[sizeof(WongWork::CMonsterDrop) == 0x2593a4 ? 1 : -1];
typedef char assert_object[sizeof(WongWork::CObjectDrop) == 0x2593a4 ? 1 : -1];
typedef char assert_clear[sizeof(WongWork::CClearReward) == 0x23b84 ? 1 : -1];
typedef char assert_dungeon[sizeof(WongWork::CDungeonDrop) == 0x18 ? 1 : -1];
