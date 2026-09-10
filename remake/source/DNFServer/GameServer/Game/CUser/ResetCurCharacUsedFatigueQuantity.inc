void CUser::ResetCurCharacUsedFatigueQuantity()
{
    m_field8eba8 = 0;   // ORIG 0x86975d4 movl $0x0,0x8eba8(%eax) 单条位移直写（&-cast 反产 add+mov 双条）
