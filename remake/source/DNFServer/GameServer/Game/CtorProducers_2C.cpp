// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CtorProducers_2C/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "CtorProducers_2C/LoadUpgradeScript.inc"
{
    // ORIG 0x0854919a：尾调 WongWork::CItemUpgradeTable::loadUpgradeScript(this+0x04,
    // path)（W 0x0854b38a，内部即 importUpgradeTable(path, table) 并透传 eax）。
    // 基类纯虚槽1 返回 int，尾透传 import 错误行号（0=成功）。
    return static_cast<CItemUpgradeTable*>(this)->loadUpgradeScript(path);
}

#include "CtorProducers_2C/Load3rdChronicleUpgradeScript.inc"
{
    // ORIG 0x085491b8：与 LoadUpgradeScript 同体（同一尾调用目标 0x0854b38a），
    // 仅符号不同；chronicle.etc 表与 Upgrade.etc 表共用表类型。
    return static_cast<CItemUpgradeTable*>(this)->loadUpgradeScript(path);
}

}  // namespace WongWork

// ============================================================================
// CItemAmplifier::loadScript —— 已移入 CItemAmplifier.h 类内定义
// （判据㉓：ORIG nm `_ZN14CItemAmplifier10loadScriptEPKcS1_` **W** 0x08376060
//  size 0x7e ⇒ 源码为类内定义；函数体逐字不变，仅符号绑定 T→W）
// ============================================================================

// ============================================================================
// WongWork::CMCAPManager —— init 与静态掉落点数表
// ============================================================================

namespace WongWork
{

// ORIG _ZN8WongWork12CMCAPManager17pointPerHackType_E @B 0x093fad20，
// nm st_size 15600 == 3900×sizeof(int)（setPointPerHackType 拷贝循环上界
// 0xf3c==3900 同证）。注：任务书曾写 [3904]，以 ORIG nm 实测尺寸为准取 [3900]。
int CMCAPManager::pointPerHackType_[3900];

#include "CtorProducers_2C/init.inc"
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

// CItemGeneratorMgr::CItemGeneratorMgr 已移入 CItemGeneratorMgr.h 类内定义
// （判据㉓：ORIG nm C1/C2 **W** 0x08374ef4 size 0x18 ⇒ 源码为类内定义；
//  函数体逐字不变，仅符号绑定 T→W）。

#include "CtorProducers_2C/initialize.inc"
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
