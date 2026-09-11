// ORIG 0x0819589e：param >= 0 时写入 +0x08。
// [b28 BM11] P223：ORIG js→空块 nop 岛 + body 尾 jmp；4.4 对 if{} 恒
// jump-thread（少 jmp+nop），复现形=「负条件 goto 尾空标签」。
void Arad_EventPeriodDataManager::setCreatePeriod_GM(int days)
{
    if (days < 0) goto L_skip;
    m_field8 = days;
L_skip:
    ;
}