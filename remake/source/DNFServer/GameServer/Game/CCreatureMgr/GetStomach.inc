// ORIG GetStomach @ 0x083384ee：(int)CStomach::GetStomachValue(+0x3c,+0x4c)
// （fnstcw/fldcw RC=11 截断，即 C++ float→int 截断转换）。
int CCreature::GetStomach()
{
    return (int)m_stomach.GetStomachValue((CREATURE_STATE)m_state,
                                          m_isGrow);
}
