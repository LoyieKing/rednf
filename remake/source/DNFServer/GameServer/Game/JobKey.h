// 全局 JobKey（df_game_r ORIG：CDungeonSuccessNode 族 map<JobKey,CSuccessJobNode> 键）。
// ORIG W 符号：JobKey::JobKey() 0x0848662a（m_job=0; m_sub=0）、
// JobKey::operator<(JobKey const&) const 0x084865e2（先 int 有符号，相等再 uchar 无符号；
// 汇编 = cmp/jge → mov1；cmp/jne → mov0；movzbl/cmpb/jge → mov0；P56 if-return 形态）。
// 与 fair_pvp::JobKey 无关（不同命名空间，mangled 前缀 _ZN6JobKey 无命名空间）。
#ifndef DNF_GAME_JOBKEY_H_
#define DNF_GAME_JOBKEY_H_

class JobKey
{
public:
    JobKey()            // ORIG W 0x0848662a
    {
        m_job = 0;
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

    int m_job;         // +0x00
    // P196：ORIG operator< 0x084865e2 对 m_sub 用 `movzbl;cmp;JGE`（有符号
    // 8 位比较），signed char 使 comparand 走有符号序；m_sub 值域 0..5 不
    // 涉 ≥0x80 分叉，语义等价。mangled 不变。
    signed char m_sub; // +0x04
};
// sizeof 0x08（int + uchar + 3 pad）

#endif  // DNF_GAME_JOBKEY_H_
