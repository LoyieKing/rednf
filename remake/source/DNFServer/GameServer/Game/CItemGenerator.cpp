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
#include "CItemGenerator.h"

#include "CDataManager.h"
#include "ServerParameterScript.h"
#include "CUser.h"
#include "CItemUpgrade_Separate.h"   // GenuineDamageUpgradeTable 完整类型 +
                                     //   importGenuineUpgradeTable 声明

#include "GameRand.h"   // get_rand_int（ORIG 0x086b1b87）

#include <new>          // placement new / operator new（getGenuineUpgradeTable）
#include <string.h>     // memset
#include <time.h>       // time(0)（CGenerateRandomNumber/GenerateRandomUpgrade 播种）

namespace WongWork
{

// [b28d-Main·检查点18] 原 `WongWork::CLuckPoint` ctor/dtor 定义已删（ORIG 无该符号；
//   该名字现为 `using ::CLuckPoint`，其 ctor/dtor 由 CClearReward.cpp 定义）。

// ============================================================================
// stGenerateRefData_t —— ORIG ctor 0x0814a706：
//   +0x00=100；+0x04=0；+0x08 vector<bool>::clear + push_back(false)×2；
//   +0x1c=1.0f；+0x20=0；+0x24=0；+0x25=0；+0x28=11；+0x2c=0；+0x30=0
// dtor 0x0814aaf0：仅析构 +0x08 vector<bool>（成员析构自动完成）。
// ============================================================================
stGenerateRefData_t::stGenerateRefData_t()
{
    m_dropRate = 100;
    // b28d-54 翻正（P238）：ORIG 0x0814a9f8 `mov $0x0,%edx; mov %edx,0x4(%eax)`
    //   常数经 edx 中转；int 字段恒折单条 movl。经 D53 的匿名 union float 视图
    //   `m_field04f = 0.0f` 复现中转（sizeof/偏移/mangled 不变）。
    m_field04f = 0.0f;
    m_forceTag.clear();
    m_forceTag.push_back(false);
    m_forceTag.push_back(false);
    m_field1c = 1.0f;
    m_premiumDropRate = 0;
    m_field24 = 0;
    m_field25 = 0;
    m_field28 = (ENUM_CHARACTERJOB)11;   // P254 enum 字段（ctor 常数写入，同宽零码）
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
//   m_count=count；count!=0 时 new CMTRand[count]（逐元素默认构造）；
//   随后第二循环逐元素播种：seed = time(0) + seedIncValue + i（函数局部
//   static seedIncValue，bss 符号 _ZZN8WongWork21CGenerateRandomNumberC1EiE
//   12seedIncValue 与 ORIG 同名），末尾 seedIncValue += count。
// dtor 0x085335d0：裸 delete []（delete[] 内置空检；ORIG 仅一次 test/je）。
// generateNumber 0x085334a4：idx 超界走 get_rand_int(range)；否则对元素
//   m_table[idx]（CMTRand，0x9c8 字节）调 randInt((unsigned long)range)。
// ============================================================================
CGenerateRandomNumber::CGenerateRandomNumber(int count)
    : m_count(count)
{
    if (count != 0)
    {
        m_table = new CMTRand[m_count];
        static int seedIncValue = 0;
        int i = 0;
        for (i = 0; i < m_count; ++i)
            m_table[i].seed(time(0) + seedIncValue + i);
        seedIncValue += i;
    }
}

CGenerateRandomNumber::~CGenerateRandomNumber()
{
    delete [] m_table;
}

int CGenerateRandomNumber::generateNumber(int idx, int range)
{
    if (m_count < idx)
        return get_rand_int(range);
    return m_table[idx].randInt((unsigned long)range);
}

// ============================================================================
// GenerateRandomUpgrade —— ORIG ctor 0x085335ee：
//   time(0) 取种子（存局部）→ operator new(0x9c8)（一个 CMTRand）→
//   CMTRand::CMTRand(this, &seed)（带种子构造，const unsigned long&）。
// dtor 0x0853364e：m_rand 非空则 operator delete，随后置 0。
// ============================================================================
GenerateRandomUpgrade::GenerateRandomUpgrade()
    : m_rand(new CMTRand(static_cast<unsigned long>(time(0))))
{
}

GenerateRandomUpgrade::~GenerateRandomUpgrade()
{
    if (m_rand)
        delete m_rand;
    m_rand = 0;
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
// ::getGenuineUpgradeTable —— ORIG T 0x0891229d 逐指令转写：
//   函数局部 static 指针（bss 符号 _ZZ22getGenuineUpgradeTablevE
//   25genuineDamageUpgradeTable 与 ORIG 同名）；首次为 0 时：
//     operator new(0x188) → 清零（ORIG gcc4.1 的 value-init 内联 rep stos；
//     gcc4.4 对 new T() 不再生成清零，故显式 memset 保持语义）→
//     placement 构造 GenuineDamageUpgradeTable（仅三 map）→
//     importGenuineUpgradeTable("Etc/Upgrade_Separate.etc", t) 失败返回 0
//     （static 保留，与 ORIG 一致：失败仅本次返回 0）。
// ============================================================================
// [本波·N77/N78 封顶] ORIG 0x0891229d（51 insns）的 value-init 清零是 4.1
//   内联 `rep stos`（5 条：mov $0/%eax、mov $0x62/%edx、%/edi、%/ecx、rep stos）；
//   4.4 对 `new T()` 依 CWG434 不发射清零、对 memset 恒发 call（探针族 /tmp 全败）
//   ⇒ 结构不可复现。本波把实现压到最接近且语义忠实的形态：
//     operator new → memset(0x188) → placement 构造 → import
//   （原先额外一层 `void* mem` 具名局部 + placement-new 的 4.4 EH 段已删）。
//   实测该件 51/52 条：残差 = rep stos 5 条 vs memset 4 条（3 准备 + call）+ 4.4
//   对 placement-new 的 landing 段；语义逐位等价（同一清零区间 + 同一 ctor）。
GenuineDamageUpgradeTable* getGenuineUpgradeTable()
{
    static GenuineDamageUpgradeTable* genuineDamageUpgradeTable = 0;
    if (genuineDamageUpgradeTable == 0)
    {
        genuineDamageUpgradeTable =
            (GenuineDamageUpgradeTable*)operator new(0x188);
        memset(genuineDamageUpgradeTable, 0, 0x188);
        new (genuineDamageUpgradeTable) GenuineDamageUpgradeTable();
        if (!importGenuineUpgradeTable("Etc/Upgrade_Separate.etc",
                                       genuineDamageUpgradeTable))
            return 0;
    }
    return genuineDamageUpgradeTable;
}

// ============================================================================
// ::CLuckPoint::UseLuckPoint —— ORIG T 0x08550a84 逐指令转写：
//   user 为空 / itemType!=1 / rarity!=2 → 返回 0；
//   level = user->get_charac_level()（第一次）；diff = grade - level；
//   rate = G_CDataManager()->m_serverParameter（+0x68 实证）
//   .getLuckPointDeductionRate(第二次 get_charac_level（P48 无 CSE）,
//                              abs(diff)（sar $0x1f/xor/sub 惯用法，内联实参）)；
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
    int rate = G_CDataManager()->m_serverParameter.getLuckPointDeductionRate(
        user->get_charac_level(), abs(grade - user->get_charac_level()));
    user->DecCurCharacLuckPoint(rate);
    return true;
}
