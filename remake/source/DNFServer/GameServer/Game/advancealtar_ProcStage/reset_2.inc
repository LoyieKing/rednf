// ----------------------------------------------------------------------------
// ORIG T 0x0812dde0（reset.md 全文）：+0x08/+0x0c/+0x10 清零、difficulty=-1、
// survivalRound=0、ProcSummonObjectMs::reset(+0x18)；type!=1 时另做：
// state=7、+0x14=0、map(+0x50) clear、survivalFirst=1、field7c=0、vector clear。
// ----------------------------------------------------------------------------
void ProcStage::reset(StageType::T type)
{
    m_field8 = 0;
    m_fieldc = 0;
    m_field10 = 0;
    m_difficulty = static_cast<StageDifficulty::T>(-1);
    m_map = 0;                    // ORIG +0x74 总是清零
    m_summonObjectMs.reset();

    if ((unsigned int)type != 1u)   // ORIG reg 形比较（P209：unsigned 载寄存器）
    {
        m_state = 7;
        m_field14 = 0;
        m_createdMapMonsters.clear();
        m_survivalFirst = 1;
        m_survivalRound = 0;      // ORIG +0x7c 仅非生存分支清零
        m_roundMaps.clear();
    }
}