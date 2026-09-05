// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// STGuildSkill：ORIG 仅导出弱内联 ctor W @0x08693f40；STGuildDBInfoOnly 的
// m_skills 实证为 5 字节步长,字段 m_field0@+0、m_field4@+4。
#ifndef DNF_GAME_STGUILDSKILL_H_
#define DNF_GAME_STGUILDSKILL_H_

// ORIG ctor 0x08693f40（W 同址 C1/C2）：movl $0xffffffff→+0 / movb $0xff→+4。
// pack(1)：STGuildDBInfoOnly::m_skills 以 5 字节步长存放 16 个 STGuildSkill（+0x45
// 起，i*5）；必须 pack(1) 否则 int+char 会被编译为 8 字节步长，字段错位。
#pragma pack(push, 1)
struct STGuildSkill
{
    STGuildSkill()          // ORIG W @0x08693f40
    {
        m_field0 = -1;
        m_field4 = static_cast<char>(0xff);
    }

    int  m_field0;          // +0x00（技能 ID）
    char m_field4;          // +0x04（等级）
};
#pragma pack(pop)

#endif  // DNF_GAME_STGUILDSKILL_H_
