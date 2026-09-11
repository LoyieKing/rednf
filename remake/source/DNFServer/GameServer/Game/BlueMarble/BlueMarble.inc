// [b28d-solo-BlueMarble AE] seed 循环修复：`volatile unsigned long seedValue`
// 强制 4.4 先求值实参再算 receiver，逐条复现 ORIG 0x080d8b22 段
// `mov -0x20(%ebp),%eax; …; lea (%ecx,%edx,1),%edx`（P316 volatile 求值序；
// 非 volatile 恒 receiver 先 + `add`）。旧 N(P75/N4) 判词（P226 语句表达式/
// 指针形/具名实参无效）作废；104=104 gap 0。ORIG 0x080d8a5c
BlueMarble::BlueMarble()
    : m_memoryPoolIndex(-1), m_state(BlueMarbleState::T_0), m_diceNumber(0), m_blueMarbleMap(0),
      m_pScript(0), m_maxBoardSize(0)
{
    m_pRands = new CMTRand[4];
    volatile unsigned long seedValue = (unsigned long)time((time_t*)0);
    for (int i = 0; i <= 3; ++i)
    {
        m_pRands[i].seed(seedValue);
    }
    m_users.clear();
    m_set.clear();
}