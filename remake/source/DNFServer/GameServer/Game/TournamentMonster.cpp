// ============================================================================
// df_game_r TournamentMonster（G2-4 竞技场怪物表项，8 字节：mobId + rate）
// ORIG：Clear 0830e4bc / Ctor 0830e4d4 / Dtor 089f1bc0（弱）。
// 布局与 CBattle_Field.h / CMap.h 的 struct TournamentMonster 一致。
// ============================================================================

struct TournamentMonster
{
    int m_mobId;  // +0x00
    int m_rate;   // +0x04

    TournamentMonster();
    ~TournamentMonster();
    void Clear();
};

TournamentMonster::TournamentMonster()
{
    Clear();
}

TournamentMonster::~TournamentMonster()
{
}

void TournamentMonster::Clear()
{
    m_mobId = 0;
    m_rate = 0;
}
