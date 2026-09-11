// stMonsterCardBindResult — C1/C2 W 0x0896dc20 range [...dc37]
//   movl $0,(%eax)；movw $0,0x4(%eax)   // 只清 +0 dword 与 +4 word
// D1/D2 W 0x08974fac size 5：push ebp; mov ebp,esp; pop ebp; ret（空体）
// ----------------------------------------------------------------------------
STExpertJobScript::stMonsterCardBindResult::stMonsterCardBindResult()
{
    ZERO_WORD(&m_field4);
    m_field0 = 0;
}