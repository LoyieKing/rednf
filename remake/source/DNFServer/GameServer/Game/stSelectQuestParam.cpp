// df_game_r 还原 —— stSelectQuestParam::stSelectQuestParam(CUser*)（ORIG T 0x83480b4）
//   ORIG mangled _ZN18stSelectQuestParamC1EP5CUser / _ZN18stSelectQuestParamC2EP5CUser。
//   反编译（see docs/class_func_reports/stSelectQuestParam/stSelectQuestParam.md）：
//     +0x00 = level                +0x04 = job
//     +0x08 = firstGrowType        +0x0c = secondGrowType
//     +0x10 = pvpGrade             +0x14 = powerSide
//     +0x1c = expertJobType        +0x20 = expertJobLevel
//     +0x24 = *( (char*)user->getCurCharacQuestR() + 0x75f0 )
//   布局与 CListTables.cpp 局部 struct stSelectQuestParam（0x18 pad + ...）同源，
//   本 TU 提供该构造函数的唯一定义；GameStubs 旧桩已删除。

#include "CUser.h"

struct stSelectQuestParam
{
    stSelectQuestParam(CUser* user);

    int  m_field00;   // +0x00
    int  m_field04;   // +0x04
    char m_field08;   // +0x08
    char m_field0c;   // +0x0c
    int  m_field10;   // +0x10
    char m_field14;   // +0x14
    char m_pad18[4];  // +0x18..0x1b
    int  m_field1c;   // +0x1c
    int  m_field20;   // +0x20
    int  m_field24;   // +0x24
    char m_pad28[0x2c];  // +0x28..0x53
};

stSelectQuestParam::stSelectQuestParam(CUser* user)
{
    m_field00 = user->get_charac_level();
    m_field04 = user->get_charac_job();
    m_field08 = user->getCurCharFirstGrowType();
    m_field0c = user->getCurCharSecondGrowType();
    m_field10 = user->get_pvp_grade();
    m_field14 = user->getPowerSide();
    m_field1c = user->GetCurCharacExpertJobType();
    m_field20 = user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp());
    UserQuest* questR = user->getCurCharacQuestR();
    m_field24 = *(int*)((char*)questR + 0x75f0);
}