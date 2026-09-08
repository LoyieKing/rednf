// 全局 JobKey（df_game_r ORIG：CDungeonSuccessNode 族 map<JobKey,CSuccessJobNode> 键）。
// ORIG W 符号：JobKey::JobKey() 0x0848662a（m_job=0; m_sub=0）、
// JobKey::operator<(JobKey const&) const 0x084865e2（先 int 有符号，相等再 uchar 无符号；
// 汇编 = cmp/jge → mov1；cmp/jne → mov0；movzbl/cmpb/jge → mov0；P56 if-return 形态）。
// 与 fair_pvp::JobKey 无关（不同命名空间，mangled 前缀 _ZN6JobKey 无命名空间）。
#ifndef DNF_GAME_JOBKEY_H_
#define DNF_GAME_JOBKEY_H_

// b28c-36: ENUM_CHARACTERJOB 以既有 guard 块内联（与 STEquipmentScript.h/Quest.h/
// CCompoundAvatar.h/GlobalScriptTutorial.h 同 guard 同成员表，幂等）——全局
// JobKey::m_job 必须为 enum 类型才能复现 ORIG operator< 的 eax→edx 中转。
#ifndef DNF_ENUM_CHARACTERJOB_DEFINED
#define DNF_ENUM_CHARACTERJOB_DEFINED
enum ENUM_CHARACTERJOB
{
    ENUM_CHARACTERJOB_0 = 0
};
#endif

class JobKey
{
public:
    JobKey()            // ORIG W 0x0848662a
    {
        m_job = ENUM_CHARACTERJOB_0;
        m_sub = 0;
    }

    bool operator<(const JobKey& rhs) const   // ORIG W 0x084865e2：jge/mov1/jmp - jne - jge ret0/mov1/mov0
    {
        if (m_job < rhs.m_job)
            return true;
        if (m_job == rhs.m_job && m_sub < rhs.m_sub)
            return true;
        return false;
    }

    // b28c-36: enum 字段——ORIG operator< 0x084865e2 第一段保留
    // `mov (%eax),%eax; mov %eax,%edx`（4.1 对 enum comparand 的中转），
    // int 字段时 4.4 折成 `mov (%eax),%edx`（fresh 27 vs ORIG 28，difflib 段错位）。
    // 探针 /tmp/b28c_l36_p4.cpp fb(enum) 与 ORIG 三段逐条同形。布局/mangled 不变。
    ENUM_CHARACTERJOB m_job;  // +0x00
    // P196：ORIG operator< 0x084865e2 对 m_sub 用 `movzbl;cmp;JGE`（有符号
    // 8 位比较），signed char 使 comparand 走有符号序；m_sub 值域 0..5 不
    // 涉 ≥0x80 分叉，语义等价。mangled 不变。
    signed char m_sub; // +0x04
};
// sizeof 0x08（int + uchar + 3 pad）

#endif  // DNF_GAME_JOBKEY_H_
