// ORIG T 0x085b9f28（SetRidableObjectState.md）：坐骑状态转发战场层（返回值穿透）。
bool CParty::SetRidableObjectState(int state, eRidableObjectState type)
{
    return reinterpret_cast<CBattle_Field*>(&m_battleField)
        ->SetRidableObjectState(state, type);
}