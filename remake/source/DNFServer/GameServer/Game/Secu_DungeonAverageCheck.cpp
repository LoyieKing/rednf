// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Secu_DungeonAverageCheck/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "Secu_HackLogCheckByParty.h"

#include <string.h>
#include <algorithm>   // std::max（ORIG 0x08279a12 调 _ZSt3maxIiERKT_S2_S2_ ×2）

#include "CParty.h"
#include "CUser.h"
#include "WarField.h"          // map_monster（m_roleType +0x08 / m_mobIndex +0x0c）
#include "CGameManager.h"      // WongWork::CHackAnalyzer::addServerHackCnt 声明

#include "Secu_DungeonAverageCheck/reset.inc"
{
    m_killInfos.clear();
}

#include "Secu_DungeonAverageCheck/Secu_DungeonAverageCheck.inc"
{
    m_party = 0;   // m_killInfos(+4) 自动构造先于 m_party(+0)=0，故置 body(ORIG 序)
}

#include "Secu_DungeonAverageCheck/dtor_Secu_DungeonAverageCheck.inc"
{
}

#include "Secu_DungeonAverageCheck/SetPartyPtr.inc"
{
    m_party = party;
    reset();
}

#include "Secu_DungeonAverageCheck/startDungeon.inc"
{
    // ORIG 0x8279490：调用 reset()（reset 内部即 clear）
    reset();
}

// [SweepB] ORIG 0x0827993c：循环体内命中即 return it（sret 拷贝），未命中末尾
// 再求一次 end() 返回（P167 MISSING vector::end×1 的根因，非 break 形）。
std::vector<SecKillMonsterInfo>::iterator
#include "Secu_DungeonAverageCheck/findKillMonsterInfo.inc"
{
    // ORIG 0x827993c：线性查找 m_monsterId 匹配项，未命中返回 end()。
    for (std::vector<SecKillMonsterInfo>::iterator it = m_killInfos.begin();
         it != m_killInfos.end(); ++it)
    {
        if ((unsigned int)it->m_monsterId == monsterId)
            return it;
    }
    return m_killInfos.end();
}

#include "Secu_DungeonAverageCheck/setKillMonsterInfo.inc"
                                                  map_monster* monster)
{
    // ORIG 0x82794d6。ROUND 为控制字截断（round toward zero），用 (int) 强转等价。
    // [P2S5] 本波战果：gap −6 → **0**（316=316），指令序列逐条同形；语义修正 1 处
    // （ORIG 四处 fistpll+`mov -0x68,%eax; mov -0x64,%edx` ⇒ 原码 double→unsigned
    // 直转，我方旧写 (unsigned int)(int)(…) 产 32 位 fistpl，负数行为不同）。
    // 残余：栈槽整体差 4 字节（我方 it −0x50 / avg1 −0x30 vs ORIG −0x4c / −0x2c，
    // 且 ORIG 独有 −0x44）⇒ 属 GCC 4.4 槽分配/复用细节。已试负结论：
    // ① it 两段式声明（先声明后赋值）→ 322（gap −6，劣化，回滚）；
    // ② 整体嵌套 `if (m_party != 0) {…}` → 315（gap +1，劣化，回滚）。
    // [P2S5] 负结论：ORIG 反编译虽呈 `if (*(int *)this != 0) { …全部… }` 嵌套形，
    // 但实测改嵌套后 316→315（gap +1）⇒ early-return 形才是 316=316 gap 0 正解。
    if (m_party == 0)
        return;
    // [P2S5] ORIG 0x082794e6 形为 `call isNamedMonster; xor $0x1,%eax; test
    // %al,%al; je …` + `movzbl 0x8(%eax),%eax; test; jne …` + `mov $1/$0,%eax;
    // test %al,%al; je <主体>`（bool 物化后 if）⇒ 原码 `if (!(isNamed ||
    // roleType != 0)) { …全部主体… }`，非 early-return（我方早退形少 1 条
    // xor、多 1 条 jmp）。
    if (!(monster->isNamedMonster() || monster->m_roleType != 0))
    {
        // [P2S5] ORIG 0x0827952e/0x0827990c 两处均**重算** `mov 0x10(%ebp),%eax;
        // mov 0xc(%eax),%eax`（无具名 monsterId 槽）⇒ P48 内联表达式。
        std::vector<SecKillMonsterInfo>::iterator it =
            findKillMonsterInfo((unsigned int)monster->m_mobIndex);
        if (it != m_killInfos.end())
        {
            if (4 < (unsigned int)it->m_count)
            {
                // [P2S5] ORIG 0x08279562/0x0827956c 与 0x082795d9/0x082795e3 有
                // `mov $0x0,%eax; mov %eax,<avg槽>`（float 局部 int-0 初始化）⇒
                // 原码为「先 0 初始化、后赋值」两段式；adj1 同样（0x08279627）。
                float avg1 = 0;
                float avg2 = 0;
                avg1 = (float)(unsigned int)it->m_sumValue1 /
                       (float)(unsigned int)it->m_count;
                avg2 = (float)(unsigned int)it->m_sumValue2 /
                       (float)(unsigned int)it->m_count;

                float adj1 = 0;
                if ((int)avg1 < 10)
                    adj1 = 1.0f;
                else
                    adj1 = 0.1f * avg1;

                // [P2S5] ORIG 0x082795f6/0x08279648/0x0827969c/0x082796ef 用
                // `fistpll`（64 位）+ `mov -0x68(%ebp),%eax; mov -0x64(%ebp),%edx`
                // ⇒ 原码是 double→unsigned 直转（非经 (int)，fistpl 为 32 位形）。
                unsigned int upper1 = (unsigned int)(0.5f + avg1 + adj1);
                unsigned int lower1 = (unsigned int)(avg1 - adj1);
                float adj2 = 0.05f * avg2;
                unsigned int upper2 = (unsigned int)(0.5f + avg2 + adj2);
                unsigned int lower2 = (unsigned int)(avg2 - adj2);

                if (m_party->m_mapHitCount > 0 &&
                    lower1 <= info->m_killValue1 && info->m_killValue1 <= upper1)
                {
                    it->m_hackCntE6 += 1;
                    for (int i = 0; i < 4; ++i)
                    {
                        // [P2S5] ORIG 0x08279615 `test %al,%al; je <增量块>`（无
                        // xor/jmp）⇒ 正条件 if；0x08279637 `mov %eax,%ebx` ⇒ user 在
                        // 寄存器（register）；getHackAnalyzer 的 this 直接是第二次
                        // get_user 结果（链式，无形参局部）。
                        if (m_party->checkValidUser(i))
                        {
                            register CUser* user = m_party->get_user(i);
                            // [P2S5] ORIG 0x08279658 `mov %eax,(%esp); call`（analyzer
                            // 无具名槽）⇒ 链式直传。
                            m_party->get_user(i)->getHackAnalyzer()->addServerHackCnt(
                                user, (WongWork::ENUM_HACKTYPE)0xe6, 1, 0, 0);
                        }
                    }
                }
                if (m_party->m_mapHitCount > 0 &&
                    lower2 <= info->m_killValue2 && info->m_killValue2 <= upper2)
                {
                    it->m_hackCntE7 += 1;
                    for (int i = 0; i < 4; ++i)
                    {
                        if (m_party->checkValidUser(i))
                        {
                            register CUser* user = m_party->get_user(i);
                            m_party->get_user(i)->getHackAnalyzer()->addServerHackCnt(
                                user, (WongWork::ENUM_HACKTYPE)0xe7, 1, 0, 0);
                        }
                    }
                }
            }
            it->m_count += 1;
            it->m_sumValue1 += (int)info->m_killValue1;
            it->m_sumValue2 += (int)info->m_killValue2;
        }
        else
        {
            m_killInfos.push_back(SecKillMonsterInfo(
                (unsigned int)monster->m_mobIndex, 1,
                (unsigned int)info->m_killValue1,
                (unsigned int)info->m_killValue2));
        }
    }
}

// ORIG T 0x08279a12：遍历 m_killInfos；每项按
//   limit = ROUND(0.5 + 0.7*m_count) - 4（rodata 0x8c02788=0.5 / 0x8c02780=0.7）
//   计算超限值 m_hackCntE6/0xE7 - max(limit,0)；任一 >0 时对 4 人队伍逐成员
//   上报 SetETC(0xe8/0xe9, job*100+grow1*10+grow2) 与
//   CHackAnalyzer::addServerHackCnt(user, 0xe8/0xe9, 超限值, SetETC 返回值, 0)。
#include "Secu_DungeonAverageCheck/CheckHackTypeEndDungeon.inc"
{
    if (m_party == 0)
        return;
    for (std::vector<SecKillMonsterInfo>::iterator it = m_killInfos.begin();
         it != m_killInfos.end(); ++it)
    {
        // ORIG 0x08279a4f/0x08279ac1：limit 表达式两次独立求值（P48，无具名缓存），
        // 每次 std::max<int>(limit,0) 独立（P167 两处调用点）。
        const int overTime = it->m_hackCntE6
            - std::max((int)(0.5 + 0.7 * (double)(unsigned int)it->m_count) - 4, 0);
        const int overExp = it->m_hackCntE7
            - std::max((int)(0.5 + 0.7 * (double)(unsigned int)it->m_count) - 4, 0);
        if (overTime > 0 || overExp > 0)
        {
            for (int i = 0; i < 4; ++i)
            {
                if (m_party->checkValidUser(i))
                {
                    CUser* user = m_party->get_user(i);
                    const int code =
                        user->get_charac_job() * 100 +
                        (int)user->getCurCharFirstGrowType() * 10 +
                        (int)user->getCurCharSecondGrowType();
                    if (overTime > 0)
                    {
                        // ORIG 0x08279bce：SetETC 写在实参位（P52 链式 receiver +
                        // R2L 实参），结果不落具名局部（直接 %esi）。
                        user->getHackAnalyzer()->addServerHackCnt(
                            user, (WongWork::ENUM_HACKTYPE)0xe8,
                            (unsigned int)overTime,
                            (unsigned int)user->SetETC(0xe8, code), 0);
                    }
                    if (overExp > 0)
                    {
                        user->getHackAnalyzer()->addServerHackCnt(
                            user, (WongWork::ENUM_HACKTYPE)0xe9,
                            (unsigned int)overExp,
                            (unsigned int)user->SetETC(0xe9, code), 0);
                    }
                }
            }
        }
    }
}

#include "Secu_DungeonAverageCheck/dungeonClear.inc"
{
    // ORIG 0x82799d6：CheckHackTypeEndDungeon() + reset()。
    CheckHackTypeEndDungeon();
    reset();
}

#include "Secu_DungeonAverageCheck/dungeonFail.inc"
{
    // ORIG 0x82799f4：CheckHackTypeEndDungeon() + reset()。
    CheckHackTypeEndDungeon();
    reset();
}
