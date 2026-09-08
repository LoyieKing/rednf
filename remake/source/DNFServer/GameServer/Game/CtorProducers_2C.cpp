// ============================================================================
// CtorProducers_2C.cpp — Ctor_Group2 续批（ContC）：升级/强化/掉落 producer 真实实现
//
// 本 TU 提供以下 ORIG 符号（全部经 docs/class_func_reports 报告 + objdump 转写，
// nm -C --defined-only /home/wangyilei/下载/df_game_r 核实真实存在）：
//   WongWork::CItemUpgrade        C1  T 0x085461c8 / LoadUpgradeScript T 0x0854919a /
//                                 Load3rdChronicleUpgradeScript T 0x085491b8
//   CItemAmplifier::loadScript    W 0x08376060（_ZN14CItemAmplifier10loadScriptEPKcS1_）
//   WongWork::CMCAPManager::init  T 0x080f8aba +
//                                 静态 int pointPerHackType_[3900] B 0x093fad20
//                                 （nm 尺寸 15600=3900×4；double-memset 忠实转写）
//   WongWork::CItemGeneratorMgr   C1 W 0x08374ef4 / initialize T 0x08533bc8
//
// 声明先行（定义待对应域批次，见交付报告 /tmp/df_batches/Ctor2_ContC.md）：
//   WongWork::CItemUpgrade::ProcUpgrade（vtable 槽0 已对齐，报告 1957 行待升级域批次）
//   CMonsterDrop/CObjectDrop/CClearReward/CWorldDrop/CDungeonDrop 各 ctor 与
//   非虚 loadScript 重载（声明见 CItemGenerator.h，定义待掉落域批次）
//   importAmplifyItem / importUpgradeTable（脚本导入域）
// ============================================================================

#include "CSystemTime.h"          // CSystemTime::getCurSec（0x080cbc9e）
#include "GlobalData.h"           // GlobalData::s_systemTime_
#include "TimerDispatcher.h"      // TimerUpdateItemGenerateStatistics::registNextTimer

#include "PvPTypes.h"             // CMTRand（0x9c8 MT19937）
#include "CItemAmplifier.h"       // upgrade_table_t / stAmplifyItemScript_t /
                                  // CItemAmplifier / import{AmplifyItem,UpgradeTable}
#include "CItemGeneratorMgr.h"    // WongWork::CItemGeneratorMgr（含 CItemGenerator.h 全链）
#include "CMCAPManager.h"         // WongWork::CMCAPManager
#include "CDataManager.h"         // WongWork::CItemUpgrade 唯一声明点

#include <cstring>
#include <ctime>

// ============================================================================
// WongWork::CItemUpgrade —— 构造与双装载入口
// ============================================================================

namespace WongWork
{

CItemUpgrade::CItemUpgrade()
{
    // ORIG 0x085461c8 逐段转写：
    //   [表区] CItemUpgradeTable(this+0x04)（upgrade_table_t 薄包装，ctor 委托
    //          upgrade_table_t 0x89000ca）——本重建沿用类内裸偏移约定，无成员建模；
    //   [基类] IItemUpgrade(this) 0x080fc834 + vptr 写入（编译器生成；槽序
    //          ProcUpgrade→LoadUpgradeScript，ORIG vtable 0x08c9d310 实证）；
    //   [+0x4ec] = 100000（randInt 热身上界）；
    //   [热身] time(NULL) 种子 → operator new(0x9c8) → CMTRand(seed) 存 +0x4e8，
    //          循环 100 次 randInt(*+0x4ec) 预热 MT19937 状态（结果弃用）。
    // b28d-lane12: ORIG 0x085461c8 实证三修（ndiff 20→0，verdict AE）——
    //   ① 基类声明序改 CItemUpgradeTable 在前（CDataManager.h；0x085461d1 `add $0x4`
    //      +call 表区先于 0x085461e2 call IItemUpgrade 实证；primary base/布局不变）；
    //   ② 去具名 pRand 局部：0x08546242 循环每次重读 m_rand（+0x4e8，P183）；
    //   ③ range 先于 seed 声明（0x0854620c seed@-0x24 / 0x0854625f range@-0x20）
    //      + 实参内联赋值 `new CMTRand(seed = time(0))`：复现 0x0854620f
    //      `lea -0x24(%ebp),%esi` 在 operator new 前装载 &seed（四形实测唯一命中：
    //      普通形/两句式/语句表达式/逗号形均 ndiff=4）。
    m_randMax = 100000;
    unsigned long range;
    unsigned long seed;
    m_rand = new CMTRand(seed = time(0));
    for (int i = 0; i < 100; ++i)
    {
        range = static_cast<unsigned long>(m_randMax);
        m_rand->randInt(range);
    }
}

int CItemUpgrade::LoadUpgradeScript(const char* path)
{
    // ORIG 0x0854919a：尾调 WongWork::CItemUpgradeTable::loadUpgradeScript(this+0x04,
    // path)（W 0x0854b38a，内部即 importUpgradeTable(path, table) 并透传 eax）。
    // 基类纯虚槽1 返回 int，尾透传 import 错误行号（0=成功）。
    return static_cast<CItemUpgradeTable*>(this)->loadUpgradeScript(path);
}

int CItemUpgrade::Load3rdChronicleUpgradeScript(const char* path)
{
    // ORIG 0x085491b8：与 LoadUpgradeScript 同体（同一尾调用目标 0x0854b38a），
    // 仅符号不同；chronicle.etc 表与 Upgrade.etc 表共用表类型。
    return static_cast<CItemUpgradeTable*>(this)->loadUpgradeScript(path);
}

}  // namespace WongWork

// ============================================================================
// CItemAmplifier::loadScript —— 强化物品脚本 + 升级表双装载
// ============================================================================

int CItemAmplifier::loadScript(const char* itemScript, const char* upgradeScript)
{
    // ORIG 0x08376060 逐段转写：
    //   line = importAmplifyItem(itemScript, m_script)；line!=0 →
    //   cMyTrace(__PRETTY_FUNCTION__,0x14,0)
    //     ("importAmplifyItem failed. line(%d)", line)（字符串 0x8c34400/0x8c2f674）
    //     返回 0；否则尾调 importUpgradeTable(upgradeScript, &m_upgrade)（asm 透传 eax，
    //     故返回 int 行号而非 bool 截断）。
    int line = importAmplifyItem(itemScript, m_script);
    if (line != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__,
                 0x14, 0)("importAmplifyItem failed. line(%d)", line);
        return 0;
    }
    return importUpgradeTable(upgradeScript, &m_upgrade);
}

// ============================================================================
// WongWork::CMCAPManager —— init 与静态掉落点数表
// ============================================================================

namespace WongWork
{

// ORIG _ZN8WongWork12CMCAPManager17pointPerHackType_E @B 0x093fad20，
// nm st_size 15600 == 3900×sizeof(int)（setPointPerHackType 拷贝循环上界
// 0xf3c==3900 同证）。注：任务书曾写 [3904]，以 ORIG nm 实测尺寸为准取 [3900]。
int CMCAPManager::pointPerHackType_[3900];

void CMCAPManager::init()
{
    // ORIG 0x080f8aba 逐指令转写。两次 memset(pointPerHackType_,0,0x3cf0) 忠实保留：
    // 中间 23 个具名下标写在 ORIG 里即为第二次 memset 前的死存储，语义照录。
    memset(pointPerHackType_, 0, sizeof(pointPerHackType_));
    pointPerHackType_[204] = 500;    // +0x330
    pointPerHackType_[111] = 250;    // +0x1bc
    pointPerHackType_[109] = 250;    // +0x1b4
    pointPerHackType_[954] = 3;      // +0xee8
    pointPerHackType_[503] = 1000;   // +0x7dc
    pointPerHackType_[403] = 20;     // +0x64c
    pointPerHackType_[806] = 200;    // +0xc98
    pointPerHackType_[207] = 250;    // +0x33c
    pointPerHackType_[301] = 250;    // +0x4b4
    pointPerHackType_[502] = 250;    // +0x7d8
    pointPerHackType_[302] = 500;    // +0x4b8
    pointPerHackType_[303] = 1000;   // +0x4bc
    pointPerHackType_[404] = 20;     // +0x650
    pointPerHackType_[103] = 3;      // +0x19c
    pointPerHackType_[104] = 3;      // +0x1a0
    pointPerHackType_[105] = 3;      // +0x1a4
    pointPerHackType_[101] = 3;      // +0x194
    pointPerHackType_[102] = 3;      // +0x198
    pointPerHackType_[201] = 500;    // +0x324
    pointPerHackType_[505] = 0;      // +0x7e4
    pointPerHackType_[306] = 0;      // +0x4c8
    pointPerHackType_[808] = 10000;  // +0xca0
    pointPerHackType_[112] = 500;    // +0x1c0
    memset(pointPerHackType_, 0, sizeof(pointPerHackType_));
}

}  // namespace WongWork

// ============================================================================
// WongWork::CItemGeneratorMgr —— 构造与 initialize（掉落生成器装配）
// ============================================================================

namespace WongWork
{

CItemGeneratorMgr::CItemGeneratorMgr()
{
    // ORIG 0x08374ef4：仅对 +0x3e0 的内嵌 CGeneratorCommonData 调用空体 ctor
    // （0x08374ebc push/mov/pop/ret），由编译器按成员初始化自动完成。
}

bool CItemGeneratorMgr::initialize()
{
    // ORIG 0x08533bc8 逐段转写（docs/class_func_reports/WongWork__CItemGeneratorMgr/
    // initialize.md）：new 八个生成器 → 前 5 个 setGeneratorType(0..4) → 依序装载
    // 脚本（任一返回行号非 0 即 throw int(line)）→ 8 生成器共享内嵌公共数据 →
    // memset 头部 0x3c0 → 注册统计定时器 → setDropRatio(1.0)。catch 打印后返 0。
    // b28d-lane12: P225 `while (line != 0) { throw line; break; }` 复现 ORIG 8 处
    //   `cmpl $0,-0x24(%ebp); setne %al; test %al,%al; je`（0x08533eb9 / 0x08533f18 /
    //   0x08533f77 / 0x08533fd1 / 0x08534030 / 0x0853408a / 0x085340e4 / 0x08534135
    //   实证；4.4 裸 if 产直 je，探针 /tmp/d12_setne_probe.cpp fB 逐条同形）⇒
    //   ndiff 178→130、gap 24→8。残余 N 类封顶（P167 差集仅 _Unwind_Resume×8，
    //   业务调用 0 缺口；ORIG 432 insn / ours 424）：
    //   ① 8 处 new 失败 cleanup ORIG 独立 `mov %eax,(%esp); call _Unwind_Resume`
    //      （0x08533c03 / 0x08533c4a / 0x08533c91 / 0x08533cd8 / 0x08533d1f /
    //      0x08533d66 / 0x08533dad / 0x08533df4），4.4.7 合并为共享 landing pad
    //      `jmp <0x8d6>`（cmp $1,%edx / je catch）——N104 族 EH 块合并，无源码形可控；
    //   ② 4 处虚调用目标寄存器 ORIG `mov (%eax),%edx … call *%edx`
    //      （0x08533efd / 0x08533f5c / 0x08534015 / 0x0853404b），4.4.7 恒 %eax——
    //      N7（P230：4.4.6 后端目标用 %edx，本 TU 未路由；换路由属 Main 决策）；
    //   ③ catch handler 相对序：ORIG 居中（0x0853416f）正常尾块置末，4.4 尾置——
    //      N104；
    //   ④ for-i 与 catch-line 槽互换（ORIG i=-0x1c / line=-0x20，4.4 相反）——
    //      N93/N98 首次使用序；i 提函数头/try 块头两形实测 130→308 劣化。
    try
    {
        m_generators[0] = new CMonsterDrop(3);   // operator new 0x2593a4
        m_generators[1] = new CObjectDrop(3);    // 0x2593a4
        m_generators[2] = new CClearReward(5);   // 0x23b84
        m_generators[3] = new CWorldDrop(2);     // 0x1f80
        m_generators[4] = new CDungeonDrop(2);   // 0x18
        m_generators[5] = new CWorldDrop(2);     // 0x1f80
        m_generators[6] = new CWorldDrop(2);     // 0x1f80
        m_generators[7] = new CWorldDrop(2);     // 0x1f80

        m_generators[0]->setGeneratorType(0);
        m_generators[1]->setGeneratorType(1);
        m_generators[2]->setGeneratorType(2);
        m_generators[3]->setGeneratorType(3);
        m_generators[4]->setGeneratorType(4);

        // 怪物掉落三脚本（ORIG 文件名「Monseter」拼写如此，忠实保留）。
        // 8 处 loadScript 检查 ORIG 均为 cmpl $0,-0x24(%ebp); setne %al; test %al,%al;
        // je（0x08533eb9 / 0x08533f18 / 0x08533f77 / 0x08533fd1 / 0x08534030 /
        // 0x0853408a / 0x085340e4 / 0x08534135 实证）；4.4 裸 if 产直 je，P225 的
        // `while (line != 0) { throw line; break; }` 精确复现 setne 物化形。
        int line = static_cast<CMonsterDrop*>(m_generators[0])->loadScript(
            "Etc/ItemDropInfo_Monseter.etc",
            "Etc/ItemDropInfo_Monster_Hell.etc",
            "Etc/ItemDropInfo_Monseter_Extra.etc");                       // T 0x085380fa 直呼
        while (line != 0)
        {
            throw line;
            break;
        }
        line = m_generators[1]->loadScript("Etc/ItemDropInfo_Object.etc");        // 虚槽 +0x04
        while (line != 0)
        {
            throw line;
            break;
        }
        line = m_generators[2]->loadScript("Etc/ItemDropInfo_ClearReward.etc");   // 虚槽 +0x04
        while (line != 0)
        {
            throw line;
            break;
        }
        line = static_cast<CWorldDrop*>(m_generators[3])->loadScript(
            "Etc/WorldDrop.etc", "Etc/WorldDropWarArea.etc");             // T 0x0853855c 直呼
        while (line != 0)
        {
            throw line;
            break;
        }
        line = m_generators[4]->loadScript("");   // DAT_08c99500 == ""（虚槽 +0x04）
        while (line != 0)
        {
            throw line;
            break;
        }
        line = static_cast<CWorldDrop*>(m_generators[5])->loadScript(
            "Etc/WorldDropPCRoom.etc", "Etc/WorldDropWarAreaPCRoom.etc");
        while (line != 0)
        {
            throw line;
            break;
        }
        line = static_cast<CWorldDrop*>(m_generators[6])->loadScript(
            "Etc/WorldDropPCRoom2.etc", "Etc/WorldDropWarAreaPCRoom.etc");
        while (line != 0)
        {
            throw line;
            break;
        }
        line = m_common.loadScript("Etc/ItemDropInfo_Common.etc");  // 内嵌 @+0x3e0
        while (line != 0)
        {
            throw line;
            break;
        }

        for (int i = 0; i < 8; ++i)
            m_generators[i]->setCommonData(&m_common);

        memset(m_head, 0, 0x3c0);   // 头部数据区清零（生成器指针/公共数据区不受影响）

        TimerUpdateItemGenerateStatistics::registNextTimer(
            GlobalData::s_systemTime_.getCurSec());
        setDropRatio(1.0f);
        return true;
    }
    catch (int line)
    {
        // ORIG __PRETTY_FUNCTION__ 字符串 0x08c9a69c 实证为
        // "bool WongWork::CItemGeneratorMgr::initialize()"；trace 行号 0x19a、flag 5。
        cMyTrace(__PRETTY_FUNCTION__, 0x19a, 5)("script load fail. line(%d)", line);
        return false;
    }
}

}  // namespace WongWork
