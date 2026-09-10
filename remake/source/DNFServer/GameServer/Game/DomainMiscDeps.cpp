// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DomainMiscDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ DomainMiscDeps 专属 TU。
// 覆盖符号（全部 T/W，地址经 ORIG df_game_r nm/objdump 核实）：
//   IndependentDropParameterScript::{load_independent_drop_list,
//       ImportIndependentDrop, importIndependentDropParameterScript}
//       0x0897c0d8 / 0x0897c398 / 0x0897c1c6
//   PvPSkillTreeParameterScript::{load_pvp_skill_tree_list,
//       importPairPvPSkillPointScript, importPvPSkillTreeParameterScript}
//       0x08a5ce28 / 0x08a5e226 / 0x08a5cf16
//   RegenerationROI::_0_Load_RegenerationRandomOptionItem 0x085f963e
//   TimerNPCMoodChange::registNextTimer(long)            0x086381ec（纯 ret）
//   TimerUpdateItemGenerateStatistics::registNextTimer(long) 0x08637318
//   OS_API::Sleep(int)                                   0x0858c4e0
//   （DNFFLib::Make_Dir 0x08108ad7 归属 ServerCommon/DNFFunctionLib.cpp，不在此重复）
// 逐函数对照 docs/class_func_reports/<类>/<方法>.md 的 ## 汇编 与
// b28d-solo-DomainMiscDeps 判词汇总（8 件，全部已读 ORIG 汇编 + Ghidra C）：
//   1. RegenerationROI::_0_Load_RegenerationRandomOptionItem 0x085f963e
//      → FIXED（P225 while+return 复现 `cmpl;setne;test;al;je` 无槽物化）。
//   2. stMonsterIndependentDrop_t::C1ERKS_ 0x08376bb2 → N90 封顶：
//      ORIG(4.1) 对 int[11] 数组成员隐式拷贝分块 5+4+1+1（组间重载 src/dst
//      基址），4.4 隐式拷贝恒单次连发。实测 6 形（隐式/拆成员/匿名 struct/
//      union 双视图/初始化列表 trivial/显式体内分块）均不能闭合：唯一能产
//      分块的是显式拷贝构造体内分块语句（探针 S6 逐条全等），但 vector 拷贝
//      构造必须在初始化列表 ⇒ 分块必落在 vector 之后，顺序与 ORIG 相反；
//      且 union 成员带用户拷贝构造被 C++ 禁止。拆真实成员会破坏动态下标
//      `m_rate[jobIdx]`（independentItemRateControl 0x08349796 单条 scaled）。
//   3. stMonsterIndependentDrop_t::aSERKS_ 0x083a3942 → N90 封顶（同上）。
//   4. load_pvp_skill_tree_list 0x08a5ce28 → FIXED（P50/P314 非 const
//      iterator 先声明后赋值 + 空名 break 守卫）。
//   5. importIndependentDropParameterScript 0x0897c1c6 → DIFF 残余
//      （p167 CLEAR 20/20）：已修 P222 正向包裹、unsigned lea 加法形、
//      make_pair<unsigned&&> 形；残余 = count 右值引用物化临时 -0x10（4.1 直传
//      count 槽地址）+ rate 槽 -0xc（4.1 -0x40）⇒ N82 槽分配族。
//   6. importPairPvPSkillPointScript 0x08a5e226 → DIFF 残余
//      （p167 CLEAR 33/33）：已修 row 声明序（帧 0xd0 与 ORIG 同）、left 两次
//      size() 内联（P48）、slotsPerJob 非 const、ScanType 失败 break+尾 return；
//      残余 2 条（N82/N93 槽向）。
//   7. ImportIndependentDrop 0x0897c398 → 真 bug 修复 + DIFF 残余
//      （p167 CLEAR 57/57）：旧代码 m_rate 索引整体错一格（rate[4..7] 循环 +
//      rate[8] + rate[9]=0），ORIG 为 rate[4] 单读(0xa5) + 循环 rate[5..8]
//      (0xab) + rate[9] 单读(0xb0) + rate[10]=0 ⇒ 掉落率/数量字段错位；
//      已补缺失的 1 个 ScanInt。残余 = 局部槽基 -0xa0 vs -0x9c（N82）。
//   8. importPvPSkillTreeParameterScript 0x08a5cf16 → DIFF 残余
//      （p167 CLEAR 116/116）：已修 operator!= 正极性（P220/P47）、
//      pair<int,int> 循环内默认构造+成员赋值+push_back(const&)（P135）、
//      去 table 引用缓存复现 3 次 perIt->second 访问、ScanType 失败 break+
//      尾 return；残余 3 条（N104 EH 尾块序 / 槽位）。
// ## 反编译 C 转写；报告缺失处以 objdump 反汇编为准并标注。

#include <algorithm>
#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <utility>
#include <vector>

#include "CDataManager.h"       // IndependentDrop/PvPSkillTree/RegenerationROI 权威布局
#include "DNFLexWrapper.h"      // loadRDARScriptFile/ScanType/ScanInt/ScanFloat
#include "LogManager.h"
#include "GlobalData.h"         // GlobalData::s_systemTime_
#include "CSystemTime.h"        // CSystemTime::getCurSec
#include "TimerQueue.h"         // G_TimerQueue/TimerEntry::InsertTimer
#include "TimerDispatcher.h"    // TimerNPCMoodChange/TimerUpdateItemGenerateStatistics
#include "NpcScript.h"

// ---- ORIG B 全局脚本文件列表（定义 InitScriptProducers_5.cpp；本 TU 引用）----
// 本地 STScriptFileList 与 STScriptFileList.cpp / InitScriptProducers_5.cpp /
// NpcScript.h 同布局（+0x00 std::map<int,std::string>, sizeof 0x18），方法调用


extern char g_independentDropScriptBaseDirectory[0x100];   // B 0x094f7840
extern STScriptFileList g_independentDropScriptFileList;   // B 0x094f7940
extern char g_pvpSkillTreeScriptBaseDirectory[0x100];      // B 0x09500240
extern STScriptFileList g_pvpSkillTreeScriptFileList;      // B 0x09500340

// ORIG T 0x08a7e610（0x416 字节）：RandomOptionItem_Regenerator 脚本解析器，
// 属 RandomOptionScript 域——本批仅按 ABI 声明引用，定义待该域批次补。
#include "DomainMiscDeps/importRegenerationRandomOptionScript.inc"
                                         const char* path);

// ===========================================================================
// OS_API::Sleep —— ORIG 0x0858c4e0：usleep(ms*1000)。
// W7_BuildHarmonize：唯一 owner 为 LinkFinalDeps.cpp（同 ORIG 符号），此处
// 原本并发批次撞车重复定义已移除，仅保留注释说明。
// ===========================================================================

// ===========================================================================
// DNFFLib::Make_Dir —— 唯一定义在 ServerCommon/DNFFunctionLib.cpp
// （ORIG 0x08108ad7：opendir 失败时 mkdir(dir,0777)；此处仅声明引用，不重复实现）。
// ===========================================================================
// 定时器注册
// ===========================================================================

// ORIG 0x086381ec：真实空操作定时注册函数（仅 ret）。
#include "DomainMiscDeps/registNextTimer.inc"
{
}


// ===========================================================================
// RegenerationROI::_0_Load_RegenerationRandomOptionItem —— ORIG 0x085f963e
// （0x51 字节）：装载再生随机选项脚本；非 0 返回值即出错行并 printf。
// ===========================================================================
// b28d-solo-DomainMiscDeps: FIXED（P225 while+return，已离开 DIFF）
#include "DomainMiscDeps/_0_Load_RegenerationRandomOptionItem.inc"
{
    int err = 0;                                            // ORIG movl $0,-0xc 预清
    err = importRegenerationRandomOptionScript(
        m_regenerator, "Etc/RandomOption/RegenerationRandomOption.etc");
    // P225/P295：ORIG `cmpl $0,-0xc; setne %al; test %al,%al; je` 无槽物化 ⇒ 源形
    // 是 while+return（g3 探针实证；具名 bool 局部必落槽 -0x9 → DIFF+1）。
    while (err != 0)
    {
        printf("Fail to Load Random Option Item Regenerator Line : %d\n", err);
        return err;
    }
    return 0;
}

// ===========================================================================
// IndependentDropParameterScript —— 独立掉落域
// ===========================================================================

// ORIG 0x0897c1c6（0x1d1 字节）逐分支转写：
//   以 g_independentDropScriptBaseDirectory 为根装载；记录索引=
//   FindIndexByFullScan(path)；[list] 段 (itemId,cumRate) 对累积进
//   info.m_list04；结束后 m_map24[索引]=(m_vec0.size(), 累积和) 并入 m_vec0。
// b28d-solo-DomainMiscDeps: DIFF 残余（p167 CLEAR 20/20；N82 槽分配）
#include "DomainMiscDeps/importIndependentDropParameterScript.inc"
    const char* path)
{
    if (!loadRDARScriptFile(g_independentDropScriptBaseDirectory, path))
    {
        return false;
    }
    std::string token;
    bool ok = false;
    IndependentDropListInfo info;
    info.m_field0 =
        g_independentDropScriptFileList.FindIndexByFullScan(path);
    int acc = 0;
    // P212/P309：ORIG 槽 itemId=-0x3c / rate=-0x40（声明序在 acc 之后）⇒ 头部声明。
    int rate;
    int itemId;
    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }
        // P222/P59 正向包裹：ORIG 0x0897c25f `test %al; je +0x65`（假→循环头，
        // 真直落内层）；`!=`+continue 反产 `je 内层; jmp 循环头`。
        if (token == "[list]")
        {
            while (true)
            {
                itemId = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                rate = ScanInt(&ok);
                if (!ok)
                {
                    return false;                  // 报告 0x0897c296 路径
                }
                // ORIG 0x0897c2a0：`mov acc,%edx; mov rate,%eax; lea (%edx,%eax,1),%eax;
                // mov %eax,acc`（4 条、unsigned 加法、两操作数均先载寄存器）。
                // [L3Free25to36 2026-09-11] `acc + rate` 形在 4.4.6 产 5 条
                // （`mov acc,%eax; mov %eax,%edx; mov rate,%eax; lea; mov`）=141 vs 142
                // （gap −1、DIFF）；**交换书写序为 `rate + acc` 后产 4 条
                // `mov rate,%edx; mov acc,%eax; lea (%edx,%eax,1),%eax; mov`**
                // ⇒ 141 vs 141（gap 0）且助记符序列与 ORIG 全等（分类 DIFF→NEAR）。
                // 唯一残差 = 两操作数寄存器互换（ORIG edx=acc，4.4 RHS-first edx=rate），
                // 属 4.1/4.4 lowering 差；如四数口径不接受 near+1/diff−1，改回
                // `acc + rate` 即可（代价：gap −1、DIFF）。
                acc = (int)((unsigned int)rate + (unsigned int)acc);
                info.m_list04.push_back(std::pair<int, int>(itemId, acc));
            }
        }
    }
    // ORIG 0x0897c2e8：`call size; mov %eax,-0xc(%ebp)` 单槽单写，随后
    // `lea -0xc(%ebp),%edx` 直传地址（-0xc 即 `unsigned int&&` 形参的**物化临时**）。
    // P0/P183：具名 `count` 槽会再产一份物化临时 ⇒ 我方 `mov %eax,-0x44; mov -0x44,%eax;
    // mov %eax,-0x10` 三条（ORIG 一条）⇒ 去掉具名局部，prvalue 直绑 rvalue 引用。
    m_map24[info.m_field0] =
        std::make_pair<unsigned int, int&>(m_vec0.size(), acc);
    m_vec0.push_back(info);
    return true;
}

// ORIG 0x0897c0d8（0xec 字节）：遍历 g_independentDropScriptFileList，
// 逐文件 importIndependentDropParameterScript；失败按行号 0x30 记日志返回 0。
#include "DomainMiscDeps/load_independent_drop_list.inc"
{
    std::map<int, std::string>::iterator it;
    it = g_independentDropScriptFileList.m_fileMap.begin();
    for (; it != g_independentDropScriptFileList.m_fileMap.end(); ++it)
    {
        const char* name = it->second.c_str();
        // b28c-33：ORIG 0897c1bd 为 `cmpl $0,-0xc; je 尾 return-true 块`（P222 正向
        // 包裹/共享尾），早返形使 4.4 反产 `jne; mov $1; jmp` 岛。
        if (name)
        {
            if (!importIndependentDropParameterScript(name))
            {
                LogManager::logFormat(
                    1, "../RDARScriptIndependentDrop.cpp",
                    __FUNCTION__, 0x30,
                    "IndependentDropParameterScript File Script Error: %s\n",
                    name);
                return false;
            }
        }
        else
        {
            break;                             // 空名 → 跳出，共享尾 return true
        }
    }
    return true;
}

// ORIG 0x0897c398（0x78f 字节）逐分支转写。返回值=出错行，0=成功：
//   0x66 装载失败；0x7d..0xcf 各 ScanInt/校验失败；0xe1 [list] 段第二列失败；
//   扫描自然结束返回 1。[independent drop] 段每条记录 16 个整数 +
//   flag(0=直接入表,1=[list] 明细,2=[list] 合并既有列表)，累积主掉率写
//   m_drop.m_mainRate，(itemId,累积率) 入 m_drop.m_pairList 后整条入 m_vecC。
//   [dungeon drop rate balance] 段每条 3 整数入 m_vec18。
// b28d-solo-DomainMiscDeps: 真 BUG 已修（m_rate 索引错一格 + 补 1 ScanInt）；
// DIFF 残余（p167 CLEAR 57/57；N82 槽基）
#include "DomainMiscDeps/ImportIndependentDrop.inc"
{
    if (!loadRDARScriptFile("", path))
    {
        return 0x66;
    }
    std::string token;
    bool ok = false;
    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;                                 // ORIG 0897caee：esi=0 ⇒ 循环外 return 0
        }
        if (token == "[independent drop]")
        {
            // [b28d-C08 widthscan 判据⑫ clr_l 5 vs 4] ORIG 0x0897c415 在本分支入口清
            // `movl $0x0,-0x44(%ebp)`，其后 0x0897c767（flag==1 `itemId=ScanInt`）与
            // 0x0897c8a9（flag==2 `itemId=m_list04[p].first`）**复用同一槽** ⇒ 原码
            // itemId 是分支级具名 int（非各 while 内声明）；
            // 0x0897c7b9/0x0897c8ec 两处 `lea -0x44(%ebp),%eax` 即引用 pair 构造的
            // 左值实参（P81 `pair<int,int>(int&,int&)`，p167 MISSING
            // `_ZNSt4pairIiiEC2IRiS2_EEOT_OT0_` ×2）。
            int itemId = 0;
            for (;;)
            {
                stIndependentDropInfo info;
                info.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    break;                         // 段结束（不压栈当前半条）
                }
                info.m_monsterIndex = ScanInt(&ok);
                if (!ok) { return 0x7d; }
                info.m_drop.m_itemIdx = ScanInt(&ok);
                if (!ok) { return 0x81; }
                info.m_drop.m_rate[0] = ScanInt(&ok);
                if (!ok) { return 0x89; }
                info.m_drop.m_rate[1] = ScanInt(&ok);
                if (!ok) { return 0x91; }
                info.m_drop.m_rate[2] = ScanInt(&ok);
                if (!ok) { return 0x99; }
                info.m_drop.m_rate[3] = ScanInt(&ok);
                if (!ok) { return 0xa1; }
                // ORIG 0897c558 单独读 m_rate[4]（错误 0xa5），循环写 m_rate[5+i]
                // （0897c591 `lea 0x4(%ebx),%edx` ⇒ 索引 5..8），再读 m_rate[9]
                // （0897c5ca，错误 0xb0）、m_rate[10]=0（0897c5ec）。旧代码整体错
                // 一格（rate[4..7] 循环 + rate[8] + rate[9]=0）= 真语义 bug。
                info.m_drop.m_rate[4] = ScanInt(&ok);
                if (!ok) { return 0xa5; }
                // [L3Free25to36 N82 复核·负结论] ORIG 0x0897c583-0x0897c594：
                // `mov -0x24(%ebp),%ebx; lea 0x4(%ebx),%edx; mov %eax,-0x8c(%ebp,%edx,4)`
                // = 基址取 &m_rate[1]（-0x8c）+ 索引 (i+4)；4.4.6 恒 `mov i,%eax;
                // lea 0x5(%eax),%ebx; mov %eax,-0x90(%ebp,%ebx,4)`（基址 m_rate[0] +
                // 索引 i+5）。落点地址两侧全等（-0x7c+4i = m_rate[5+i]），仅常量拆
                // 分点不同；换书写序 `i + 5` 实测零变化 ⇒ 4.1 独有寻址形，封顶。
                for (int i = 0; i < 4; ++i)
                {
                    info.m_drop.m_expect[i] = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0xab;
                    }
                }
                info.m_drop.m_limit = ScanInt(&ok);
                if (!ok)
                {
                    return 0xb0;
                }
                info.m_drop.m_mainRate = 0;
                info.m_drop.m_pairList.clear();
                info.m_drop.m_rangeMin = ScanInt(&ok);
                if (!ok) { return 0xb7; }
                info.m_drop.m_rangeMax = ScanInt(&ok);
                if (!ok) { return 0xbb; }
                if (info.m_drop.m_rangeMin > info.m_drop.m_rangeMax)
                {
                    return 0xbe;
                }
                info.m_drop.m_levelFilter = ScanInt(&ok);
                if (!ok) { return 0xc3; }
                int flag = ScanInt(&ok);
                if (flag != 0 && flag != 1 && flag != 2)
                {
                    LogManager::logFormat(
                        1, "../RDARScriptIndependentDrop.cpp",
                        __FUNCTION__, 0xca,
                        "flag wrong number(%d)", flag);
                    return 0xcb;
                }
                if (!ok)
                {
                    return 0xcf;
                }

                std::string listToken;
                if (flag == 1)
                {
                    if (ScanType(listToken, true))
                    {
                    if (listToken == "[list]")
                    {
                        while (true)
                        {
                            itemId = ScanInt(&ok);
                            if (!ok)
                            {
                                break;
                            }
                            int rate = ScanInt(&ok);
                            if (!ok)
                            {
                                return 0xe1;       // ORIG esi=0xe1/ebx=0 → 错误返回
                            }
                            // ORIG 0x0897c7a3 累积槽 -0x68 = info+0x34 = **m_rate[10]**
                            // （`movl $0x0,-0x68(%ebp)` 就在 0x0897c5ec 清零同一槽）
                            // ⇒ 累积目标与 pair 第二元同为 m_rate[10]，非 m_rate[9]。
                            info.m_drop.m_mainRate += rate;
                            info.m_drop.m_pairList.push_back(
                                std::pair<int, int>(
                                    itemId, info.m_drop.m_mainRate));
                        }
                    }
                    }
                }
                else if (flag == 2)
                {
                    if (ScanType(listToken, true))
                    {
                    if (listToken == "[list]")
                    {
                        while (true)
                        {
                            // P212/P309：ORIG 0x0897c828 `movb $0x0,-0x19(%ebp)` 在
                            // 0x0897c832 `call ScanInt` **之前**（槽 -0x19 深于 idx 的
                            // -0x18）⇒ found 声明在 idx 之前。
                            bool found = false;
                            int idx = ScanInt(&ok);
                            if (!ok)
                            {
                                break;             // 结束本段明细（ORIG 0897c83a）
                            }
                            for (int e = 0; e < (int)m_vec0.size(); ++e)
                            {
                                // ORIG 0x0897c86c `cmp -0x18(%ebp),%eax; sete %al` = 正极性
                                // `==`；搜索体后 0x0897c949 `cmpb $0x0,-0x19(%ebp);
                                // je <++e>` = **体尾独立 `if (found) break;`**（非命中点
                                // 内 break）⇒ 原码 `if (==) {...; found = true;} if (found) break;`。
                                if (m_vec0[e].m_field0 == idx)
                                {
                                // [b28d-C08 C05④ 第2步试形·已回退] ORIG 0x0897c964
                                // `cmp -0x14(%ebp),%eax; setg %al; test %al,%al; jne` 的
                                // setcc 物化，改 `int p=0; while(p<size())` 后**确实复现**
                                // （OURS setcc 3 = ORIG 3：setle + setg×2，逐条同形同槽），
                                // 但 gap +9→**+10**、difflib equal_ratio 0.828 不变
                                // （本件 519~529 条 >500，N127 台账失真）⇒ **无净收益**
                                // 按纪律回退，维持 for 形（P25 的 while 形钥匙对本件
                                // 只换来 setcc 形态、未换 gap/对齐）。
                                for (int p = 0;
                                     p < (int)m_vec0[e].m_list04.size();
                                     ++p)
                                {
                                    itemId =
                                        m_vec0[e].m_list04[p].first;
                                    int rate =
                                        m_vec0[e].m_list04[p].second;
                                    info.m_drop.m_mainRate += rate;
                                    info.m_drop.m_pairList.push_back(
                                        std::pair<int, int>(
                                            itemId,
                                            info.m_drop.m_mainRate));
                                }
                                found = true;
                                }
                                if (found)
                                {
                                    break;             // ORIG 命中后跳出线性搜索
                                }
                            }
                            (void)found;
                        }
                    }
                    }
                }
                m_vecC.push_back(info);
            }
        }
        // ORIG 0x0897c9fa（dead 分支 ebx==0）落点 = 0x0897ca0a（"[dungeon drop rate
        // balance]" 比较）⇒ 段循环 break 后**仍走第二个比较** ⇒ 原码为两个**并列 if**
        // 而非 else-if（else-if 时 4.4.6 把 dead 落点折成循环头，多一对 jmp+nop 岛）。
        if (token == "[dungeon drop rate balance]")
        {
            for (;;)
            {
                stIndependentDropRateControl rc;
                rc.m_itemId = ScanInt(&ok);
                if (!ok) { break; }
                rc.m_field04 = ScanInt(&ok);
                if (!ok) { break; }
                rc.m_field08 = ScanInt(&ok);
                if (!ok) { break; }
                m_vec18.push_back(rc);
            }
        }
    }
    // ORIG 0x0897caef `mov $0x0,%esi; jmp 0x0897cb11`（循环耗尽路径）⇒ 成功返回 **0**
    // （CDataManager.h:2580 注释「0=成功」；调用点 CDataManager.cpp:2718 以 `!= 0` 判错）。
    return 0;
}

// ===========================================================================
// PvPSkillTreeParameterScript —— PvP 技能树域
// ===========================================================================

// ORIG 0x08a5cf16（0xc43 字节）逐分支转写：
//   [level]/[job index]/[grow type index]/[awakening type] 维护当前键与等级；
//   [skill] 段：明细对入新 PvPSkillTreeValue.m_vec00 且同步写 m_map30[key]
//   的加/减技能表（缺项建 FAIR 默认值）；段尾把 value 插 m_map0[key][level]
//   （重复 grade/键 → 失败）。[static basic skill] 段要求键已存在，明细对追加
//   到既有 value.m_vec0c 并同样更新 m_map30。
// b28d-solo-DomainMiscDeps: DIFF 残余（p167 CLEAR 116/116；N104 EH 尾块序）
// [L3Free25to36 已落 · 复核 2026-09-11] clr_l 两处 ORIG 清零已补：
//   ①0x08a5d0e8 `movl $0x0,-0x10(%ebp)` → `int unusedSlot = 0;`（[skill] 分支入口，
//     value C1Ev(0x08a5d0e3) 之后、pair C1Ev(0x08a5d0f8) 之前；P232/P171 死局部照抄）。
//   ②0x08a5d7d2 `movl $0x0,-0xc(%ebp)` → `std::vector<...>* vec = 0;`（指针槽入口清零，
//     0x08a5d856/0x08a5d920 两次覆写、0x08a5d9b4 读）。
//   实落读数：777 vs 777（gap 0，改前 777 vs 774），difflib ratio 0.236→0.518。
// [L3Free25to36 残余封顶] 帧整体平移 0x10（我方全部槽浅 0x10：token -0x1f0 vs ORIG
//   -0x200），且浅区槽集不同（ORIG 独有 -0x18/-0x20/-0x2c/-0x30/-0x6c/-0x70/-0x8c/
//   -0xa8/-0xb0/-0xe8/-0xec/-0xf0/-0x10c，我方独有 -0x44/-0x84/-0xa0/-0xbc/-0xc0/
//   -0x104）⇒ 属局部变量集/物化点差异（含 map/pair 临时），非单旋钮可复原，记 N104。
//   `int curLevel = 0;` 初始化为我方独有 1 条（ORIG 无；去掉则 776=gap −1），保留以
//   维持 gap 0 且避免未初始化读。
#include "DomainMiscDeps/importPvPSkillTreeParameterScript.inc"
    const char* path)
{
    if (!loadRDARScriptFile(g_pvpSkillTreeScriptBaseDirectory, path))
    {
        return false;
    }
    std::string token;
    bool ok = false;
    PvPSkillTreeKey key;
    int curLevel = 0;
    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;                                 // ORIG 08a5db1a：跳循环外 return 1
        }
        if (token == "[level]")
        {
            curLevel = ScanInt(&ok);
            if (!ok)
            {
                return false;
            }
        }
        else if (token == "[job index]")
        {
            key.m_a = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                return false;
            }
        }
        else if (token == "[grow type index]")
        {
            key.m_b = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                return false;
            }
        }
        else if (token == "[awakening type]")
        {
            key.m_c = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                return false;
            }
        }
        else if (token == "[static basic skill]")
        {
            std::map<PvPSkillTreeKey,
                     std::map<int, PvPSkillTreeValue> >::iterator treeIt =
                m_map0.find(key);
            // ORIG 08a5d76c：operator!= 正极性 + else{return false}（P220/P47）。
            if (treeIt != m_map0.end())
            {
                std::map<int, PvPSkillTreeValue>::iterator gradeIt =
                    treeIt->second.find(curLevel);
                // ORIG 08a5d8??：local_10 = &gradeIt->second.m_vec0c 指针缓存。
                // ORIG 0x08a5d7d2 `movl $0x0,-0xc(%ebp)`：指针槽入口清零（该槽即
                // 下面 vec 的指针缓存，0x08a5d856/0x08a5d920 两次覆写、0x08a5d9b4 读）。
                std::vector<std::pair<int, int> >* vec = 0;
                if (gradeIt != treeIt->second.end())
                {
                    vec = &gradeIt->second.m_vec0c;
                }
                else
                {
                    PvPSkillTreeValue fresh;
                    std::pair<std::map<int, PvPSkillTreeValue>::iterator,
                              bool> ins = treeIt->second.insert(
                        std::make_pair(curLevel, fresh));
                    gradeIt = ins.first;
                    vec = &gradeIt->second.m_vec0c;
                }
                while (true)
                {
                    // ORIG 08a5d95f：循环内 pair<int,int> 默认构造 + 成员赋值 +
                    // push_back(const&)（P135 左值形）。
                    std::pair<int, int> entry;
                    entry.first = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    entry.second = ScanInt(&ok);
                    if (!ok)
                    {
                        break;                     // ORIG 08a5da22 双失败出口
                    }
                    vec->push_back(entry);
                    std::map<PvPSkillTreeKey,
                             FAIR_PVP_SKILL_TREE_SKILL_PER_POINT>::iterator
                        perIt = m_map30.find(key);
                    if (perIt != m_map30.end())
                    {
                        // ORIG 三次独立 perIt->second.m_addSkills 访问。
                        std::map<int, int>::iterator slot =
                            perIt->second.m_addSkills.find(entry.first);
                        if (slot != perIt->second.m_addSkills.end())
                        {
                            slot->second = entry.second;
                        }
                        else
                        {
                            perIt->second.m_addSkills.insert(
                                std::make_pair(entry.first, entry.second));
                        }
                    }
                    else
                    {
                        return false;              // ORIG 08a5d9c8 路径
                    }
                }
            }
            else
            {
                return false;
            }
        }
        else if (token == "[skill]")
        {
            PvPSkillTreeValue value;
            // ORIG 0x08a5d0e8 `movl $0x0,-0x10(%ebp)`：value C1Ev(0x08a5d0e3) 之后、
            // pair C1Ev(0x08a5d0f8) 之前；全函数该槽**仅此一次写、零读**（P232/P171
            // 死局部照抄，槽序 -0x10 与 ORIG 同深）。
            int unusedSlot = 0;
            while (true)
            {
                std::pair<int, int> entry;
                entry.first = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                entry.second = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                value.m_vec00.push_back(entry);
                std::map<PvPSkillTreeKey,
                         FAIR_PVP_SKILL_TREE_SKILL_PER_POINT>::iterator
                    perIt = m_map30.find(key);
                if (perIt != m_map30.end())
                {
                    std::map<int, int>::iterator slot =
                        perIt->second.m_addSkills.find(entry.first);
                    if (slot != perIt->second.m_addSkills.end())
                    {
                        slot->second = entry.second;
                    }
                    else
                    {
                        perIt->second.m_addSkills.insert(
                            std::make_pair(entry.first, entry.second));
                    }
                }
                else
                {
                    FAIR_PVP_SKILL_TREE_SKILL_PER_POINT fresh;
                    fresh.m_addSkills.insert(
                        std::make_pair(entry.first, entry.second));
                    m_map30.insert(
                        std::make_pair(key, fresh));
                }
            }
            std::map<PvPSkillTreeKey,
                     std::map<int, PvPSkillTreeValue> >::iterator treeIt =
                m_map0.find(key);
            if (treeIt != m_map0.end())
            {
                std::pair<std::map<int, PvPSkillTreeValue>::iterator,
                          bool> ins = treeIt->second.insert(
                    std::make_pair(curLevel, value));
                if (!ins.second)
                {
                    return false;                  // 重复 grade
                }
            }
            else
            {
                std::map<int, PvPSkillTreeValue> inner;
                std::pair<std::map<int, PvPSkillTreeValue>::iterator,
                          bool> ins = inner.insert(
                    std::make_pair(curLevel, value));
                if (!ins.second)
                {
                    return false;                  // ORIG local_f0!=1 路径
                }
                m_map0.insert(
                    std::make_pair(key, inner));
            }
        }
    }
    return true;
}

// ORIG 0x08a5e226（0x42b 字节）逐分支转写：
//   [skill point]：ScanFloat 四舍五入为 int（fstcw/fldcw 就近偶数舍入），
//   按 (jobIdx, 0..0x36) 分行插入 m_map18；残行（非 0 非 0x37）→ 失败。
//   [version]：单字节版本写 +0x50。[init skill tree]：(key,val) 入 m_map54，
//   重复键 → 失败。
// b28d-solo-DomainMiscDeps: DIFF 残余（p167 CLEAR 33/33；N82/N93 槽向）
#include "DomainMiscDeps/importPairPvPSkillPointScript.inc"
    const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    std::string token;
    bool ok = false;
    // ORIG 0x08a5e26a：row 构造在 slotIdx/jobIdx/slotsPerJob 预清之前 ⇒ 声明序在前。
    std::map<int, int> row;
    int slotIdx = 0;
    int jobIdx = 0;
    // ORIG 0x08a5e3a0 / 0x08a5e2f2 两处比较段均为 `mov -0xc(%ebp),%edx; cmp %edx,%eax`
    // （槽→寄存器两载）。L3Free25to36 实测：**变量保持 int + 比较式去掉
    // `static_cast<int>(row.size())`** ⇒ 混合符号（unsigned size() vs int 槽）触发
    // 4.4 先把槽转 unsigned 载入 %edx ⇒ 逐条 AE（275/275）。
    // 反例：`static_cast<int>(row.size()) == slotsPerJob` 两侧同型 ⇒ 折
    // `cmp -0xc(%ebp),%eax` 单条（gap +2）；`unsigned` 槽同样折 mem cmp，均无效。
    int slotsPerJob = 0x37;
    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;                                 // ORIG 08a5e5eb：跳循环外 return true
        }
        if (token == "[skill point]")
        {
            for (;;)
            {
                // ORIG：先 (int) 转换（x87 fistpl 截断）后检查 ok；lrintf 是
                // round-to-nearest，对 .5 值语义不同（真差异）。
                int rounded = static_cast<int>(ScanFloat(&ok));
                if (!ok)
                {
                    break;
                }
                row.insert(std::make_pair(slotIdx++, rounded));
                if (row.size() == slotsPerJob)
                {
                    m_map18.insert(
                        std::make_pair(jobIdx++, row));
                    row.clear();
                    slotIdx = 0;
                }
            }
            // ORIG 0x08a5e470/0x08a5e485 两次独立 `call size`（P48：O0 无 CSE，
            // 两次调用=源码两次；具名 left 多一槽使全帧 +8）。
            if (row.size() != slotsPerJob &&
                static_cast<int>(row.size()) != 0)
            {
                return false;                      // 残行 → 失败（ORIG 08a5e49a）
            }
        }
        else if (token == "[version]")
        {
            // ORIG 0x08a5e4d4：`call ScanInt; mov %eax,%edx; mov %dl,0x50(%eax)`
            // 无具名 ver 局部（先存后查 ok，P204 顺序）。
            m_field50 = (char)ScanInt(&ok);
            if (!ok)
            {
                return false;
            }
        }
        else if (token == "[init skill tree]")
        {
            // ORIG 0x08a5e516/0x08a5e520 两槽预清（key=-0xb4 / val=-0xb8）⇒
            // 头部声明 + 循环内赋值；循环形 `for(;;){key=Scan; if(!ok)break; ...}`。
            int key = 0;
            int val = 0;
            for (;;)
            {
                key = ScanInt(&ok);
                if (!ok)
                {
                    break;                         // 段结束回主扫描
                }
                val = ScanInt(&ok);
                if (!ok)
                {
                    return false;
                }
                if (!m_map54.insert(std::make_pair(key, val)).second)
                {
                    return false;                  // ORIG local_2c!=1 路径
                }
            }
        }
    }
    return true;
}

// ORIG 0x08a5ce28（0xec 字节）：遍历 g_pvpSkillTreeScriptFileList，逐文件
// importPvPSkillTreeParameterScript；失败按行号 0x45 记日志返回 0。
// b28d-solo-DomainMiscDeps: FIXED（P50/P314 非 const iterator + 空名守卫）
#include "DomainMiscDeps/load_pvp_skill_tree_list.inc"
{
    // P50 复活/P314：ORIG 0x08a5ce28 先 `_Rb_tree_iterator::C1Ev`（-0x14）再
    // `it = begin()`（begin sret -0x1c，单 dword 拷贝）；非 const iterator
    // （operator!=/operator->/operator++ 均非 const_iterator 实例，P96/P270）。
    std::map<int, std::string>::iterator it;
    it = g_pvpSkillTreeScriptFileList.m_fileMap.begin();
    for (; it != g_pvpSkillTreeScriptFileList.m_fileMap.end(); ++it)
    {
        const char* name = it->second.c_str();
        // ORIG 0x08a5ce73：`cmpl $0,-0xc; je +0xe5(nop) → 尾 mov $1` 空名跳出。
        if (name == 0)
        {
            break;
        }
        if (!importPvPSkillTreeParameterScript(name))
        {
            LogManager::logFormat(
                1, "../RDARScriptPvPSkillTree.cpp",
                __FUNCTION__, 0x45,
                "PvPSkillTreeParameterScript File Script Error: %s\n",
                name);
            return false;
        }
    }
    return true;
}
