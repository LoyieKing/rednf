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
    // [b28d-solo-DropLayoutDeps] 封 N18a（ORIG 0x085345b6）：new 表达式实参地址
    // 求值序跨版本——ORIG 先 `lea -0x1c(%ebp),%esi` 跨 operator new 保活，4.4 恒
    // 分配后求实参（`lea …%edx` 在 new 之后）。本轮实测 P226 裸语句表达式
    // `new CMTRand(*({&seed;}))` 无效（同 43 条，序不变）。语义等价。
    if (m_rand != 0)
        delete m_rand;
    unsigned long seed = time(0);
    register unsigned long* pSeed = &seed;
    m_rand = new CMTRand(*pSeed);
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
    // [b28d-solo-DropLayoutDeps] 203/202 条；已修：删 pairBytes/maxOffset/target
    // 局部（ORIG 全程表达式重算，P0/P183）、end() 不缓存（ORIG end 在条件块）、
    // insert 直接构造 value_type（非 make_pair，P192）、grades 用 const 指针
    //（ORIG 调 ZNK…5beginEv，消迭代器转换 ctor）、m_slots 索引用 level（ORIG 原码
    // 行为，P171 照抄）。残差=帧/槽号（N82）+ 虚调 IsRandomOption 寄存器角色（N7）。
    // 报告 generateTable.md（T 0x0853466c）逐循环转写。槽址 =
    // type*0x7110+rarity*0x12d8+level*24 ⇒ m_slots[type][rarity][level]。
    const __gnu_cxx::hash_map<int, CItem*>* grades =
        G_CDataManager()->m_itemList->getItemGradeList();
    for (int type = 1; type <= 4; ++type)
    {
        for (int rarity = 0; rarity <= 5; ++rarity)
        {
            for (int level = 1; level <= 200; ++level)
            {
                unsigned int cumulative = 0;
                // ORIG：无 pairBytes/maxOffset/target 局部（每次重算表达式）。
                int offset = -(int)(*reinterpret_cast<const unsigned char(*)[2]>(
                                  &range->m_range[level]))[0];   // -(u8)min
                for (; offset < (int)(*reinterpret_cast<const unsigned char(*)[2]>(
                                     &range->m_range[level]))[1]; ++offset)
                {
                    if (level + offset > 200)         // cmp $0xc8 jg → 终止整行
                        break;
                    for (__gnu_cxx::hash_map<int, CItem*>::const_iterator
                             it = grades[level + offset].begin();
                         it != grades[level + offset].end(); ++it)
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
                        m_slots[type][rarity][level].insert(   // ORIG 用 level（非 target）
                            std::pair<const unsigned int, int>(
                                cumulative, item->get_index()));
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
    // [b28d-solo-DropLayoutDeps] 封 N18a（ORIG 0x08534c1a）：与 CItemGenRateTable::init
    // 同款（lea -0x1c,%esi 跨 new 的 4.1 求值序不可造）。语义等价。
    if (m_rand != 0)
        delete m_rand;
    unsigned long seed = time(0);
    register unsigned long* pSeed = &seed;
    m_rand = new CMTRand(*pSeed);
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
    // [b28d-solo-DropLayoutDeps] 227/229 条（ORIG 227），p167 CLEAR（17=17）。
    // 已修：同 CItemGenRateTable::generateTable（去局部缓存、end 条件块、直接构造
    // pair、const grades、m_blocks 用 level）+ 数组指针解引用视图复现 ORIG 0x08534d2f
    // `add %eax,%eax; add 0xc(%ebp),%eax; movzbl (%eax)`/`0x1(%eax)` disp 折叠。
    // 残差 N（2 处）：ORIG 谓词调用点 0x08534ee4/0x08534ee7 为
    // `call; xor $1; test %al,%al; jne`（P292 ⇒ callee bool），我方 uchar 声明恒产
    // `call; test; sete; test`。已试 P248 档① callee bool 化（CItem.h 两声明+定义）：
    // 调用点转正、本函数 227/227，但两谓词定义体由 7 条→9 条（4.4 对 char 数组元素
    // 转 bool 加 `test; setne` 归一化，ORIG 4.1 无）⇒ TU 净 -1 AE，已回退。
    // 语义等价（非零判定）。
    // 报告 generateTable.md（T 0x08534cd0）：五重循环 a(0..4) b(0..2) type(1..4)
    // rarity(0..5) level(1..200)；槽址 a*0x69ff0+b*0x23550+type*0x7110+
    // rarity*0x12d8+level*24。过滤要求 isSpecialMonsterDropItem /
    // isDropItemDungeonDifficulty(a) / isDropItemDungeonType(b) 全真，无随机选项分支。
    const __gnu_cxx::hash_map<int, CItem*>* grades =
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
                        int offset = -(int)(*reinterpret_cast<
                                          const unsigned char(*)[2]>(
                                          &range->m_range[level]))[0];
                        for (; offset < (int)(*reinterpret_cast<
                                             const unsigned char(*)[2]>(
                                             &range->m_range[level]))[1]; ++offset)
                        {
                            if (level + offset > 200)
                                break;
                            for (__gnu_cxx::hash_map<int, CItem*>::const_iterator
                                     it = grades[level + offset].begin();
                                 it != grades[level + offset].end(); ++it)
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
                                m_blocks[a][b][type][rarity][level].insert(
                                    std::pair<const unsigned int, int>(
                                        cumulative, item->get_index()));
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
    // [b28d-solo-DropLayoutDeps] N82/N112 封顶（ORIG 0x08535344，279/282）。
    // p167 CLEAR（23=23）。已修：for(;;)+break（顶部条件头）、i<rows+1（ORIG
    // add$1/setg 形）、fill 值初始化 stGenItemProb_t()（内联清零，消 p167 EXTRA
    // memset）、idx 循环前预清（P198①）、全头部声明（P212）。残差=槽分配（N82附：
    // 4.4 聚合 fill 恒占最深槽 -0x50..-0x39，ORIG 按声明序落 -0x44..-0x2d 且 token
    // -0x48/ok -0x49 更深；fill 声明移首/移尾两形实测槽集合不变）+ m_range[0]
    // movw vs 两条 movb（N78 族）。语义等价。
    if (!loadRDARScriptFile("", path))
        return 0x302;
    std::string token;
    bool ok = false;
    stGenItemProb_t fill;                    // 全部局部提到函数头（P212）
    int idx = 0;                             // ORIG 循环前预清（-0x28，P198①）
    int rows;
    int i;
    int j;
    size_t jd;
    int* e;
    int k;
    int lvl;
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[basis of rarity dicision]")
        {
            rows = ScanInt(&ok);
            if (!ok)
                return 0x311;
            for (i = 1; i < rows + 1; ++i)   // ORIG add$1/setg 形
            {
                for (j = 0; j <= 5; ++j)
                {
                    m_rarityBasis[i][j] = ScanInt(&ok);
                    if (!ok)
                        return 0x319;
                }
            }
        }
        else if (token == "[drop prob count]")
        {
            fill = stGenItemProb_t();
            m_probs.resize(ScanInt(&ok), fill);
            if (!ok)
                return 0x320;
        }
        else if (token == "[dungeon difficulty drop prob]")
        {
            for (jd = 0; jd < m_probs.size(); ++jd)
            {
                e = &m_probs[jd].v[0];
                e[0] = ScanInt(&ok);
                if (!ok)
                    return 0x328;
                e[1] = ScanInt(&ok);
                if (!ok)
                    return 0x32a;
                for (k = 0; k <= 4; ++k)
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
            for (lvl = 1; lvl <= 200; ++lvl)
            {
                idx = ScanInt(&ok);
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
    // [b28d-solo-DropLayoutDeps] N82/N76 封顶（ORIG 0x085373ee，379/383）。
    // p167 CLEAR（28=28）。已修：for(;;)+break、fill 值初始化 stGenItemProb_t()
    // （消 p167 EXTRA memset）、idx 循环前预清、全头部声明（P212）。残差=①槽分配
    // （N82附：ours 最深 -0x64 含 fill 聚合，ORIG -0x5d 且 token/ok 更深）；
    // ②`e[1] = ScanInt(&ok)` store 序 ORIG RHS-first `mov -S,%edx; mov %eax,0x4(%edx)`
    // vs 4.4 LHS-first `lea 0x4(%eax),%ebx` 跨 call 缓存（N76）；已试 P211 异型化
    // （e→unsigned int* + cast）反劣 +1 条，回退。语义等价。
    if (!loadRDARScriptFile("", path))
        return 0x5fb;
    std::string token;
    bool ok = false;
    stGenItemProb_t fill;
    int idx = 0;
    size_t jd;
    int* e;
    int k;
    int i;
    int j;
    int i2;
    int j2;
    int b;
    int j3;
    int t;
    int j4;
    int lvl;
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[drop prob count]")
        {
            fill = stGenItemProb_t();
            m_probs.resize(ScanInt(&ok), fill);
            if (!ok)
                return 0x609;
        }
        else if (token == "[drop prob]")
        {
            for (jd = 0; jd < m_probs.size(); ++jd)
            {
                e = &m_probs[jd].v[0];
                e[0] = ScanInt(&ok);
                if (!ok)
                    return 0x611;
                e[1] = ScanInt(&ok);
                if (!ok)
                    return 0x613;
                for (k = 0; k <= 4; ++k)
                {
                    e[2 + k] = ScanInt(&ok);
                    if (!ok)
                        return 0x617;
                }
            }
        }
        else if (token == "[basis of rarity dicision]")
        {
            for (i = 1; i <= 4; ++i)
            {
                for (j = 0; j <= 5; ++j)
                {
                    m_rarityBasis[i][j] = ScanInt(&ok);
                    if (!ok)
                        return 0x623;
                }
            }
        }
        else if (token == "[party member drop bonusrate]")
        {
            for (i2 = 0; i2 <= 4; ++i2)
            {
                for (j2 = 0; j2 <= 3; ++j2)
                {
                    m_partyBonus[i2][j2] = ScanFloat(&ok);
                    if (!ok)
                        return 0x62e;
                }
            }
        }
        else if (token == "[dungeon difficulty drop bonusrate]")
        {
            for (b = 0; b <= 4; ++b)
            {
                for (j3 = 0; j3 <= 4; ++j3)
                {
                    m_dungeonBonus[b][j3] = ScanFloat(&ok);
                    if (!ok)
                        return 0x63d;
                }
            }
        }
        else if (token == "[monster type drop bonusrate]")
        {
            for (t = 0; t <= 4; ++t)
            {
                for (j4 = 0; j4 <= 3; ++j4)
                {
                    m_monsterTypeBonus[t][j4] = ScanFloat(&ok);
                    if (!ok)
                        return 0x648;
                }
            }
        }
        else if (token == "[item drop ref table]")
        {
            m_range.m_range[0] = 0;
            for (lvl = 1; lvl <= 200; ++lvl)
            {
                idx = ScanInt(&ok);
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
    // [b28d-solo-DropLayoutDeps] N82/N76 封顶（ORIG 0x08537938，545/550）。
    // p167 CLEAR（42=42）。已修：for(;;)+break、fill 值初始化（消 p167 EXTRA
    // memset）、count 内联实参、[gold quantity] 展开为 ORIG 的 4 次独立 ScanInt
    // （原循环少 3 个调用点，p167 MISSING -3）、全头部声明（P212）。残差=①槽分配
    // （N82附：ours 最深 -0x68 含 fill 聚合、ok -0x49；ORIG -0x61/ok -0x61 相邻）；
    // ②`e[1] = ScanInt(&ok)` store 序 ORIG RHS-first vs 4.4 LHS-first（N76，与
    // CSpecialMonsterDrop 同型）；③多分支出口 jmp/nop 岛数（P63/N108）。语义等价。
    if (!loadRDARScriptFile("", path))
        return 0x66b;
    std::string token;
    bool ok = false;
    stGenItemProb_t fill;
    int idx = 0;
    size_t jd;
    int* e;
    int k;
    int i;
    int j;
    int i2;
    int j2;
    int b;
    int j3;
    int t;
    int j4;
    int lvl;
    int k2;
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[drop prob count]")
        {
            fill = stGenItemProb_t();
            m_probs.resize(ScanInt(&ok), fill);
            if (!ok)
                return 0x679;
        }
        else if (token == "[drop prob]")
        {
            for (jd = 0; jd < m_probs.size(); ++jd)
            {
                e = &m_probs[jd].v[0];
                e[0] = ScanInt(&ok);
                if (!ok)
                    return 0x681;
                e[1] = ScanInt(&ok);
                if (!ok)
                    return 0x683;
                for (k = 0; k <= 4; ++k)
                {
                    e[2 + k] = ScanInt(&ok);
                    if (!ok)
                        return 0x687;
                }
            }
        }
        else if (token == "[basis of rarity dicision]")
        {
            for (i = 1; i <= 4; ++i)
            {
                for (j = 0; j <= 5; ++j)
                {
                    m_rarityBasis[i][j] = ScanInt(&ok);
                    if (!ok)
                        return 0x693;
                }
            }
        }
        else if (token == "[party member drop bonusrate]")
        {
            for (i2 = 0; i2 <= 4; ++i2)
            {
                for (j2 = 0; j2 <= 3; ++j2)
                {
                    m_partyBonus[i2][j2] = ScanFloat(&ok);
                    if (!ok)
                        return 0x69e;
                }
            }
        }
        else if (token == "[dungeon difficulty drop bonusrate]")
        {
            for (b = 0; b <= 4; ++b)
            {
                for (j3 = 0; j3 <= 4; ++j3)
                {
                    m_dungeonBonus[b][j3] = ScanFloat(&ok);
                    if (!ok)
                        return 0x6ad;
                }
            }
        }
        else if (token == "[monster type drop bonusrate]")
        {
            for (t = 0; t <= 4; ++t)
            {
                for (j4 = 0; j4 <= 3; ++j4)
                {
                    m_monsterTypeBonus[t][j4] = ScanFloat(&ok);
                    if (!ok)
                        return 0x6b8;
                }
            }
        }
        else if (token == "[item drop ref table]")
        {
            m_range.m_range[0] = 0;
            for (lvl = 1; lvl <= 200; ++lvl)
            {
                idx = ScanInt(&ok);
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
            for (k2 = 0; k2 <= 3; ++k2)
            {
                m_conditionRate[k2].rate = (unsigned short)ScanInt(&ok);
                if (!ok)
                    return 0x6e5;
                m_conditionRate[k2].value = (unsigned short)ScanInt(&ok);
                if (!ok)
                    return 0x6e9;
            }
        }
        else if (token == "[gold quantity]")
        {
            // ORIG 展开 4 次 ScanInt（各带独立错误码，非循环）。
            m_goldQuantity[0] = (unsigned short)ScanInt(&ok);
            if (!ok)
                return 0x6f0;
            m_goldQuantity[1] = (unsigned short)ScanInt(&ok);
            if (!ok)
                return 0x6f4;
            m_goldQuantity[2] = (unsigned short)ScanInt(&ok);
            if (!ok)
                return 0x6f8;
            m_goldQuantity[3] = (unsigned short)ScanInt(&ok);
            if (!ok)
                return 0x6fc;
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
    // [b28d-solo-DropLayoutDeps] AE（ORIG 0x0853a968）：194/194 条、帧 0x3c、
    // EH 槽 -0x30/-0x2c 全等（仅 .o 重定位伪差）。修法=m_goldCardCost 清零由
    // memset 改 ORIG 的 for 循环（N78/P214③：4.1 内联逐元素 store）。
    // ORIG 0x0853a968：基类 → vptr 0x08c9a700 → 7×vector(+0x18 步进 0xc，第 7 个在
    // +0x60) → 表 ctor(+0x2a8) → 两个空白卡 vector(+0x23b40/+0x23b78) →
    // 金卡费用表 [201] 清零 → +0x23b38=0.0f → +0x23b3c=0 → +0x23b74=0 →
    // memset(+0x23b4c,0,0x28) → set_disable_drop_random_option_item()。
    // 成员对象由编译器按声明序完成；标量/清零在此补齐。
    for (int i = 0; i <= 200; ++i)          // ORIG 循环清零（非 memset）
        m_goldCardCost[i] = 0;
    m_goldCardCreateRate = 0.0f;
    m_goldCardBlankTotal = 0;
    m_pcRoomBlankTotal = 0;
    memset(m_rewardRatePerMap, 0, sizeof(m_rewardRatePerMap));
    m_table.set_disable_drop_random_option_item();
}

int CClearReward::loadScript(char const* path)
{
    // [b28d-solo-DropLayoutDeps] N82 封顶（ORIG 0x085392c0，657/660；§6.2 超大件
    // 三件套：p167 CLEAR 61=61、gap=3、槽位集仅深度差 0x55→0x4d）。已修：for(;;)+break
    //（顶部条件头）。残差=局部槽序（4.4 首次使用序/聚合优先 vs 4.1 声明序，
    // N82/P112；section/token 槽 -0x55/-0x54 vs -0x4d/-0x4c）+ 多分支出口岛数
    //（P63/N108）。语义等价。
    if (!loadRDARScriptFile("", path))
        return 0x943;
    std::string section;
    std::string token;
    bool ok = false;
    for (;;)
    {
        if (!ScanType(section, true))
            break;
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
    // [b28d-solo-DropLayoutDeps] AE（ORIG 0x08534456；verdict 以 check_tu_routed
    // clean 口径为准，尾部对齐 nop 由 §6.1 strip_nop_padding 剥离；game_func_report
    // --no-compile 报 101/102 属 N124 伪差）。p167 CLEAR（10=10）。已修：for(;;)+
    // goto 尾标签（P164/P223）、idx 循环前预清（P198①）、条件 operator==（p167 判据）、
    // return 0x1f8 块移至循环出口后。形实验记录：if/else 正形为最优；break+循环后
    // return、标签置循环内、#pragma optimize(no-align-labels/loops/jumps/align-labels=1)
    // 均致块序错或条数 -1，已回退。
    if (!loadRDARScriptFile("", path))
        return 0x1d8;
    std::string token;
    int idx = 0;                             // ORIG 循环前预清（-0x10，P198①）
    bool ok = false;
    for (;;)
    {
        if (!ScanType(token, true))
            goto ok_done;
        if (token == "[gold drop ref table]")   // ORIG 用 operator==（p167 判据）
        {
            m_goldGenTable[0].m_base = 0;
            m_goldGenTable[0].m_randAmp = 0;
            for (int i = 1; i <= 200; ++i)
            {
                idx = ScanInt(&ok);          // 写入下标来自脚本首列（ORIG 同形）
                if (!ok)
                    return 0x1ec;
                m_goldGenTable[idx].m_base = ScanInt(&ok);
                if (!ok)
                    return 0x1f0;
                m_goldGenTable[idx].m_randAmp = ScanInt(&ok);
                if (!ok)
                    return 500;              // 0x1f4
            }
        }
        else
        {
            goto fail_ref_table;
        }
    }
fail_ref_table:
    return 0x1f8;
ok_done:
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

bool CItem::isSpecialMonsterDropItem()
{
    return (unsigned char)m_drop.m_enable;  // ORIG 0x8539e00：movzbl 0xb4(%eax) 直返
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
__attribute__((target("arch=i486")))
int CAutoMarketConditionsControl::GetGoldDropAmountRate(int level, int gold)
{
    // [b28d-solo-DropLayoutDeps] N 封顶（ORIG 0x082f89bc，98/99；本函数级
    // `__attribute__((target("arch=i486")))` 消 x87 比较 lowering，N85 突破口）。
    // 已修：①真语义 bug——比较改 Ghidra C 实证的 `rate < 0.0`（原 `!(rate>0.0)`
    // 在 rate==0/NaN 时错误回退 base；ORIG 0x82f8a20 `test $0x45,%ah; sete` 实证）；
    // ②logCount/trace 段移入 m_field00!=0 分支（ORIG `je +0x11c`）；③P243 乘序
    // (1-base)*goldPerLevel；④register bool logIt（P231 免槽，槽组 -0x14/-0x10/-0x24；
    // 删 register 帧 0x54→0x64 全槽漂）。残差 N18a/N7：静态 logCount 自增 ORIG
    // `lea 0x1(%ecx),%edx; mov %edx,addr` vs 4.4 `mov/add/mov`（+1 条，`+=1`/
    // `1+x`/`x+1` 三形实测同）+ bool 局部 %al vs %bl（跨版本寄存器分配）。
    int result = 0;                         // ORIG prolog 后立即清零（-0x14）
    double rate;                            // ORIG 不预初始化（仅 else 路径赋值）
    if (m_field00 != 0)
    {
        // rate = (1-base)*cond[level]/(-1000) + 1；出 (0,1] 则回退 base。
        // ORIG fldl base→fsubp 先于 fildl cond[level]（P243 书写序）。
        rate = ((1.0 - m_goldBaseRate) * (double)m_goldPerLevel[level]) / -1000.0 + 1.0;
        if (rate < 0.0 || rate > 1.0)   // Ghidra C 实证：严格小于（非 <=）
            rate = m_goldBaseRate;
        result = (int)((double)gold * rate);         // ORIG fistpl 直接截断（无 +0.5）
        static int logCount = 0;            // ORIG bss _ZZ...GetGoldDropAmountRateEiiE8logCount
        register bool logIt = (logCount % 1000 == 0);
        logCount += 1;
        if (logIt)
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__, 0xce, 0);
            trace("goldDropAmountRate(%lf) gold(%d), baseGold(%d)", rate, result, gold);
        }
    }
    else
    {
        result = gold;                      // 未启用折算：原样返回（ORIG 尾部块）
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
