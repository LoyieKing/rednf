// STGuildDBInfoOnly.h — 公会 DB 结构权威定义（唯一声明点）
//
// [Lane37CUser 2026-09-10 解环] 自 CUser.h 原文逐字节迁出（类型/成员/注释/断言零改动）：
//   SkillSlot.h 的 `master_new_guild_skill(..., const STGuildDBInfoOnly&, ...)` 需要该
//   完整类型，而 SkillSlot.h ⇄ CUser.h 存在环 ⇒ 提到本叶子头供两侧 include。
//   本头只依赖 STGuildSkill.h（叶子头），不引入任何回指 CUser.h 的 include。
#ifndef DNF_STGUILDDBINFOONLY_H_
#define DNF_STGUILDDBINFOONLY_H_

#include "STGuildSkill.h"  // STGuildSkill 完整类型（m_skills[16]）

#pragma pack(push, 1)  // 公会 DB 结构按 ORIG pack(1)（+0x9a int 非对齐；HEAD 原有，恢复）
struct STGuildDBInfoOnly
{
    STGuildDBInfoOnly();
    char m_guildName[0x17];           // +0x00
    int m_masterNo;                   // +0x17
    unsigned char m_fundLevel;        // +0x1b
    unsigned short m_ability;         // +0x1c（ctor 写 1）
    int m_field1E;                    // +0x1e
    unsigned short m_memberCount;     // +0x22
    int m_guildPoint;                 // +0x24
    unsigned char m_guildRank;        // +0x28
    unsigned int m_fundExp;           // +0x29
    unsigned char m_subMasterCnt;     // +0x2d
    // ORIG IsSubGuildMaster 0x0866c424 折出 `0xe(%eax,%edx,4)` 索引形⇒ 元素必须为 int
    // （char[]+cast 恒出 base+shl 双步）；sizeof 0x14 不变，消费点仅 CUser.cpp。
    int m_subMasterData[5];           // +0x2e（pack(1) 区非对齐 int 副会长表）
    unsigned short m_guildSkillPoint; // +0x42
    unsigned char m_skillLearnCnt;    // +0x44
    // 判据㉜/㊹：ORIG STGuildDBInfoOnly::C1Ev 0x08693f56 以 5B 步长循环 call
    //   STGuildSkill::C1Ev（ctor 写 +0=0xffffffff/+4=0xff）；char[0x50]+手写 memset
    //   会产出 EXTRA memset/MISSING STGuildSkill::C2Ev（p167 实证）。改真类型后由
    //   隐式成员构造复现 ORIG 循环。sizeof 0x50 不变（pack(1) 5B×16）。
    STGuildSkill m_skills[16];        // +0x45（16×STGuildSkill 5B）
    unsigned char m_field95;          // +0x95（SetGuildInfoProxy 0x0865cd43 单步 movzbl 直落⇒uchar；char 反产双步，CUOC7 技巧5）
    int m_powerSecedeTime;            // +0x96
    unsigned int m_powerWarPoint;     // +0x9a（ORIG Decrease 0x082301bc copy-sub 形需 uint）
    char m_agitExist;                 // +0x9e
    unsigned char m_powerJoinCount;   // +0x9f
    unsigned int m_fundMoney;         // +0xa0
    char m_masterName[0x15];          // +0xa4
    int m_fieldB9;                    // +0xb9
};
#pragma pack(pop)
static_assert(sizeof(STGuildDBInfoOnly) == 0xbd, "STGuildDBInfoOnly size");

#endif  // DNF_STGUILDDBINFOONLY_H_
