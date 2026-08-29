// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// STGuildSkill：ORIG 仅导出弱内联 ctor W @0x08693f40；
// 其余成员/布局无证据，sizeof 未证实故不加 static_assert。
#ifndef DNF_GAME_STGUILDSKILL_H_
#define DNF_GAME_STGUILDSKILL_H_

// ORIG ctor 0x08693f40（W 同址 C1/C2）：movl $0xffffffff→+0 / movb $0xff→+4。
struct STGuildSkill
{
    STGuildSkill()          // ORIG W @0x08693f40
    {
        m_field0 = -1;
        m_field4 = static_cast<char>(0xff);
    }

    int  m_field0;          // +0x00
    char m_field4;          // +0x04
};

#endif  // DNF_GAME_STGUILDSKILL_H_
