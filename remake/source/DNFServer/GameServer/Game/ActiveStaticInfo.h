#ifndef DNF_GAME_ACTIVESTATICINFO_H_
#define DNF_GAME_ACTIVESTATICINFO_H_

// G2 占位（TODO：真实实现属怪物系统批次）。
// 尺寸 0x2f0：map_monster 内嵌成员 @ +0x38..+0x328（ctor 0x8150ec6 / dtor 0x8150ef6）。

class ActiveStaticInfo
{
public:
    ActiveStaticInfo();
    ~ActiveStaticInfo();
    void clear();
    int getHpMax();
    int getMpMax();
    int m_level;
    int m_hpBase;
    float m_hpRatio;
    int m_mpBase;
    float m_mpRatio;
    int m_flag14;
    int m_attack;
    float m_attackRatio;
    int m_magicAttack;
    float m_magicAttackRatio;
    int m_defense;
    float m_defenseRatio;
    int m_magicDefense;
    float m_magicDefenseRatio;
    int m_elemAttack[4];
    float m_elemAttackRatio[4];
    int m_elemDefense[4];
    float m_elemDefenseRatio[4];
    int m_attr17Base[17];
    float m_attr17Ratio[17];
    int m_statBase0; float m_statRatio0;
    int m_statBase1; float m_statRatio1;
    int m_statBase2; float m_statRatio2;
    int m_statBase3; float m_statRatio3;
    int m_statBase4; float m_statRatio4;
    int m_statBase5; float m_statRatio5;
    int m_statBase6; float m_statRatio6;
    int m_statBase7; float m_statRatio7;
    int m_statBase8; float m_statRatio8;
    int m_statBase9; float m_statRatio9;
    int m_statBase10; float m_statRatio10;
    int m_statBase11; float m_statRatio11;
    int m_statBase12; float m_statRatio12;
    int m_statBase13; float m_statRatio13;
    int m_statBase14; float m_statRatio14;
    int m_statBase15; int m_statBase16; int m_statBase17; int m_statBase18;
    int m_statBase19; int m_statBase20; int m_statBase21; int m_statBase22;
    int m_statBase23; int m_statBase24; int m_statBase25; int m_statBase26;
    int m_statBase27; int m_statBase28; int m_statBase29; int m_statBase30;
    float m_statRatio15; int m_statBase31; int m_statBase32; int m_statBase33;
    int m_statBase34; float m_statRatio16; int m_statBase35; int m_statBase36;
    float m_statRatio17; int m_statBase37;
    int m_table1[18]; int m_table2[18]; int m_table3[18];
    int m_f2cc; int m_f2d0; int m_f2d4; int m_f2d8;
    float m_f2dc; float m_f2e0; int m_f2e4; int m_f2e8; float m_f2ec;
};

#endif  // DNF_GAME_ACTIVESTATICINFO_H_
