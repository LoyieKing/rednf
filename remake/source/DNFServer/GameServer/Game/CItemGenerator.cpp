// ============================================================================
// CItemGenerator.cpp — WongWork 掉落生成器域权威实现（唯一 TU）
//
// 提供以下 ORIG 符号（全部经 df_game_r nm/objdump 核实）：
//   _ZN8WongWork10CLuckPointC1Ev / D1Ev            0x08550884 / 0x085508ec
//   _ZN8WongWork19stGenerateRefData_tC1Ev / D1Ev   0x0814a706 / 0x0814aaf0
//   _ZN8WongWork18stGenerateResult_tC1Ev / D1Ev    0x0814ab06 / 0x0814ab4c
//   _ZN8WongWork21CGenerateRandomNumberC1Ei / D1Ev 0x085334ea / 0x085335d0
//   _ZN8WongWork21CGenerateRandomNumber14generateNumberEii 0x085334a4
//   _ZN8WongWork21GenerateRandomUpgradeC1Ev / D1Ev 0x085335ee / 0x0853364e
//   _ZN8WongWork14CItemGeneratorC1Ei / D1Ev        0x08533b3e / 0x08533b9a
#include "CItemGenerator.h"

#include "CDataManager.h"
#include "ServerParameterScript.h"
#include "CUser.h"

#include "GameRand.h"   // get_rand_int（ORIG 0x086b1b87）

namespace WongWork
{

// ============================================================================
// CLuckPoint —— ORIG ctor：vector<CUser*> @+0x00 → map<int,int> @+0x0c
// ============================================================================
CLuckPoint::CLuckPoint() {}
CLuckPoint::~CLuckPoint() {}

// ============================================================================
// stGenerateRefData_t —— ORIG ctor 0x0814a706：
//   +0x00=100；+0x04=0；+0x08 vector<bool>::clear + push_back(false)×2；
//   +0x1c=1.0f；+0x20=0；+0x24=0；+0x25=0；+0x28=11；+0x2c=0；+0x30=0
// dtor 0x0814aaf0：仅析构 +0x08 vector<bool>（成员析构自动完成）。
// ============================================================================
stGenerateRefData_t::stGenerateRefData_t()
{
    m_dropRate = 100;
    m_field04 = 0;
    m_forceTag.clear();
    m_forceTag.push_back(false);
    m_forceTag.push_back(false);
    m_field1c = 1.0f;
    m_premiumDropRate = 0;
    m_field24 = 0;
    m_field25 = 0;
    m_field28 = 11;
    m_field2c = 0;
    m_field30 = 0;
}

stGenerateRefData_t::~stGenerateRefData_t()
{
}

// ============================================================================
// stGenerateResult_t —— ORIG ctor 0x0814ab06：
//   vector<Inven_Item>(+0x00) 默认构造 → CLuckPoint(+0x0c) 构造。
// dtor 0x0814ab4c：逆序析构两成员（成员析构自动完成）。
// ============================================================================
stGenerateResult_t::stGenerateResult_t() {}
stGenerateResult_t::~stGenerateResult_t() {}

// ============================================================================
// CGenerateRandomNumber —— ORIG ctor 0x085334ea：
//   m_count=count；count!=0 时 new CMTRand[count]（逐元素默认构造）。
//   （ORIG 在 count==0 时跳过分配、表指针保持未初始化；此处防御性置 0，
//     与 dtor 的判空 delete[] 行为一致。）
// dtor 0x085335d0：表非空则 operator delete[]。
// generateNumber 0x085334a4：idx 超界走 get_rand_int(range)；否则对元素
//   m_table[idx]（CMTRand，0x9c8 字节）调 randInt((unsigned long)range)。
// ============================================================================
CGenerateRandomNumber::CGenerateRandomNumber(int count)
    : m_table(0), m_count(count)
{
    if (m_count != 0)
        m_table = new CMTRand[m_count];
}

CGenerateRandomNumber::~CGenerateRandomNumber()
{
    if (m_table)
        delete[] m_table;
}

int CGenerateRandomNumber::generateNumber(int idx, int range)
{
    if (m_count < idx)
        return get_rand_int(range);
    return m_table[idx].randInt((unsigned long)range);
}

// ============================================================================
// GenerateRandomUpgrade —— ORIG ctor 0x085335ee：
//   time(0) 取种子 → operator new(0x9c8)（即一个 CMTRand）→ 以种子做一次
//   初始化调用（0x080cba2a 未具名符号，按 CMTRand 播种处理 [INFERENCE]）。
// ============================================================================
GenerateRandomUpgrade::GenerateRandomUpgrade()
    : m_rand(new CMTRand)
{
}

GenerateRandomUpgrade::~GenerateRandomUpgrade()
{
    if (m_rand)
        delete m_rand;
}

// ============================================================================
// CItemGenerator —— ORIG ctor 0x08533b3e：
//   vptr（编译器自动）→ m_random(+0x08, CGenerateRandomNumber(count)) →
//   m_upgrade(+0x10)。注意 ORIG 不初始化 +0x04 m_common 与 +0x14 type。
// dtor 0x08533b9a：重置 vptr → 析构 m_upgrade → 析构 m_random（非虚析构，
//   成员析构顺序由编译器保证）。
// 6 个 W inline 方法体均来自 objdump 直译（空实现/字段读写）。
// ============================================================================
CItemGenerator::CItemGenerator(int count)
    : m_random(count)
{
}

CItemGenerator::~CItemGenerator()
{
}

void CItemGenerator::setDropRatio(float)
{
    // ORIG 0x0853a3f0：基类空实现（派生类覆写）。
}

int CItemGenerator::loadScript(char const*)
{
    // ORIG 0x0853a3f6：mov $0x0,%eax —— 返回错误行 0（成功）。
    return 0;
}

void CItemGenerator::generateRandomItem(stGenerateRefData_t const&, stGenerateResult_t&)
{
    // ORIG 0x0853a400：基类空实现（派生类覆写）。
}

void CItemGenerator::generateSpecificItem(stGenerateRefData_t const&, stGenerateResult_t&)
{
    // ORIG 0x0853a406：基类空实现（派生类覆写）。
}

void CItemGenerator::setCommonData(CGeneratorCommonData* common)
{
    m_common = common;  // ORIG 0x0853a3e2：mov 0xc(%ebp),%edx; mov %edx,0x4(%eax)
}

CGeneratorCommonData* CItemGenerator::getGeneratorCommonData()
{
    return m_common;  // ORIG 0x085beff4
}

void CItemGenerator::setGeneratorType(int type)
{
    m_generatorType = type;  // ORIG 0x0853a40c：写 +0x14
}

int CItemGenerator::getGeneratorType()
{
    return m_generatorType;  // ORIG 0x0853a41a：读 +0x14
}

}  // namespace WongWork

// ============================================================================
// ::CLuckPoint::UseLuckPoint —— ORIG T 0x08550a84 逐指令转写：
//   user 为空 / itemType!=1 / rarity!=2 → 返回 0；
//   rate = G_CDataManager()->m_serverParameter.getLuckPointDeductionRate(
//             user->get_charac_level(), grade)（ORIG add $0x68 实证）；
//   user->DecCurCharacLuckPoint(rate)；返回 1。
// ============================================================================
bool CLuckPoint::UseLuckPoint(CUser* user, int itemType, int rarity, int grade)
{
    if (user == 0)
        return false;
    if (itemType != 1)
        return false;
    if (rarity != 2)
        return false;
    int level = user->get_charac_level();
    int rate = G_CDataManager()->m_serverParameter.getLuckPointDeductionRate(
        level, grade);
    user->DecCurCharacLuckPoint(rate);
    return true;
}
