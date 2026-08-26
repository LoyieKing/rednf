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
    int m_dieCnt;                          // +0x0c
    int m_field10;                         // +0x10
    int m_field14;                         // +0x14
    void init();                           // ORIG W 0x81512ec
    void setUseSkillMaterial(unsigned int tick);   // ORIG W 0x830e72c
    // 击杀节奏反外挂链（kill_monster 消费；ORIG 偏移语义见下方实现注释）。
    void setLastMobDieTime(unsigned int tick);     // ORIG W 0x830e796
    int checkLongTermMobDie(unsigned int tick, unsigned int limit);  // ORIG W 0x830e7a4
    void setUncontinuallyMobDieCnt(unsigned int cnt);      // ORIG W 0x830e7dc
};

void stMapMonsterKillChecker_t::init()
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
void stMapMonsterKillChecker_t::setUseSkillMaterial(unsigned int tick)
{
    m_useSkillMaterialTime = tick;
    ++m_field14;
}

// ---- 击杀节奏反外挂链（kill_monster +0x1585..+0x18a8 消费）----
// ORIG W _ZN25stMapMonsterKillChecker_t15setLastMobDieTimeEj @ 0x830e796：
//   mov 0xc(%ebp),%edx; mov %edx,0x10(%eax)   → 写 +0x10（本头 m_field10 槽；
//   ORIG 中该槽即"上次击杀时刻"，与 dieMonster 用的 +0x04 是两个独立槽位）。
void stMapMonsterKillChecker_t::setLastMobDieTime(unsigned int tick)
{
    m_field10 = tick;
}

// ORIG W _ZN25stMapMonsterKillChecker_t19checkLongTermMobDieEjj @ 0x830e7a4：
//   mov 0x10(%eax),%eax; test; je ret         → last(+0x10)==0 直接返回 m_0c
//   tick - last ≥ limit（无符号 jb 判 <）时 ++m_0c
//   返回 *(int*)(this+0xc)（本头 m_dieCnt 槽；ORIG 该槽存"非连续计数"）
int stMapMonsterKillChecker_t::checkLongTermMobDie(unsigned int tick,
                                                   unsigned int limit)
{
    if (m_field10 != 0 && tick - m_field10 >= limit)
        ++m_dieCnt;
    return m_dieCnt;
}

// ORIG W _ZN25stMapMonsterKillChecker_t25setUncontinuallyMobDieCntEj @ 0x830e7dc：
//   mov %edx,0xc(%eax)                        → 写 +0x0c（m_dieCnt 槽）
void stMapMonsterKillChecker_t::setUncontinuallyMobDieCnt(unsigned int cnt)
{
    m_dieCnt = static_cast<int>(cnt);
}

struct stMapPlayInfo_t
{
    unsigned int m_startTick;    // +0x00
    unsigned int m_clearTick;    // +0x04
    int m_useItemType2Count;     // +0x08
    int m_useItemOtherCount;     // +0x0c
    void init();                 // ORIG W 0x815132c
};

void stMapPlayInfo_t::init()
{
    m_startTick = 0;
    m_clearTick = 0;
    m_useItemType2Count = 0;
    m_useItemOtherCount = 0;
}
