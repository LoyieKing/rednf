// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 FieldSmallStructs/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— 战斗/地图小结构基础方法（独立 TU）。
//   布局与 WarField.h 权威定义一致（stMapMonsterKillChecker_t/stMapPlayInfo_t）：
//     stMapMonsterKillChecker_t::init()  ORIG W 0x81512ec（清空 6×int=0x18 字节）
//     stMapPlayInfo_t::init()            ORIG W 0x815132c（清空 4×int=0x10 字节）
//   ORIG mangled: _ZN25stMapMonsterKillChecker_t4initEv /
//                 _ZN15stMapPlayInfo_t4initEv
// 注：本文件原有全局 stGenerateResult_t 镜像（_ZN18stGenerateResult_tC1Ev 等）
// 在 ORIG 中不存在——ORIG 实际符号为 WongWork 域 _ZN8WongWork18stGenerateResult_t*，
// 现由权威 TU CItemGenerator.cpp 提供，此处镜像系发明符号，已删除。
#include <cstring>

struct stMapMonsterKillChecker_t
{
    unsigned int m_useSkillMaterialTime;   // +0x00
    unsigned int m_lastMobDieTime;         // +0x04
    int m_uncontinuallyMobDieCnt;          // +0x08
    unsigned int m_dieCnt;                 // +0x0c（与 WarField.h 权威同步：同型赋值保 ORIG store 展开序）
    unsigned int m_field10;                // +0x10（同上）
    int m_field14;                         // +0x14
    void init();                           // ORIG W 0x81512ec
    void setUseSkillMaterial(unsigned int tick);   // ORIG W 0x830e72c
    // 击杀节奏反外挂链（kill_monster 消费；ORIG 偏移语义见下方实现注释）。
    void setLastMobDieTime(unsigned int tick);     // ORIG W 0x830e796
    int checkLongTermMobDie(unsigned int tick, unsigned int limit);  // ORIG W 0x830e7a4
    void setUncontinuallyMobDieCnt(unsigned int cnt);      // ORIG W 0x830e7dc
};

#include "FieldSmallStructs/init.inc"
{
    m_useSkillMaterialTime = 0;
    m_lastMobDieTime = 0;
    m_uncontinuallyMobDieCnt = 0;
    m_dieCnt = 0;
    m_field10 = 0;
    m_field14 = 0;
}

// ORIG W _ZN25stMapMonsterKillChecker_t19setUseSkillMaterialEj @ 0x830e72c。
// 无独立函数报告；本实现经 ORIG objdump（0x830e72c..0x830e748）逐指令核实，
// 调用方报告 CBattle_Field/onUseItem.md 已完整核对：
//   mov 0xc(%ebp),%edx; mov %edx,(%eax)        → m_useSkillMaterialTime = tick
//   mov 0x14(%eax),%eax; lea 0x1(%eax),%edx;
//   mov %edx,0x14(%eax)                        → m_field14 += 1
#include "FieldSmallStructs/setUseSkillMaterial.inc"
{
    m_useSkillMaterialTime = tick;
    ++m_field14;
}

// ---- 击杀节奏反外挂链（kill_monster +0x1585..+0x18a8 消费）----
// ORIG W _ZN25stMapMonsterKillChecker_t15setLastMobDieTimeEj @ 0x830e796：
//   mov 0xc(%ebp),%edx; mov %edx,0x10(%eax)   → 写 +0x10（本头 m_field10 槽；
//   ORIG 中该槽即"上次击杀时刻"，与 dieMonster 用的 +0x04 是两个独立槽位）。
#include "FieldSmallStructs/setLastMobDieTime.inc"
{
    m_field10 = tick;
}

// ORIG W _ZN25stMapMonsterKillChecker_t19checkLongTermMobDieEjj @ 0x830e7a4：
//   mov 0x10(%eax),%eax; test; je ret         → last(+0x10)==0 直接返回 m_0c
//   tick - last ≥ limit（无符号 jb 判 <）时 ++m_0c
//   返回 *(int*)(this+0xc)（本头 m_dieCnt 槽；ORIG 该槽存"非连续计数"）
#include "FieldSmallStructs/checkLongTermMobDie.inc"
                                                   unsigned int limit)
{
    if (m_field10 != 0 && tick - m_field10 >= limit)
        ++m_dieCnt;
    return m_dieCnt;
}

// ORIG W _ZN25stMapMonsterKillChecker_t25setUncontinuallyMobDieCntEj @ 0x830e7dc：
//   mov %edx,0xc(%eax)                        → 写 +0x0c（m_dieCnt 槽）
#include "FieldSmallStructs/setUncontinuallyMobDieCnt.inc"
{
    m_dieCnt = cnt;   // 字段已 unsigned（WarField.h），同型赋值保 ORIG store 展开序
}

struct stMapPlayInfo_t
{
    unsigned int m_startTick;    // +0x00
    unsigned int m_clearTick;    // +0x04
    int m_useItemType2Count;     // +0x08
    int m_useItemOtherCount;     // +0x0c
    void init();                 // ORIG W 0x815132c
};

#include "FieldSmallStructs/init_1.inc"
{
    m_startTick = 0;
    m_clearTick = 0;
    m_useItemType2Count = 0;
    m_useItemOtherCount = 0;
}
