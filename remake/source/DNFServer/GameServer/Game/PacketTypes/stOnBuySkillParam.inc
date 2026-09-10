stOnBuySkillParam::stOnBuySkillParam()
    : m_4(0), m_350(-1)
{  // ORIG 0x8230a04：m_infos[70] 隐式逐元素 buySkillInfoC1（GCC 循环），尾部标量与
   // memset(this+8,0,0x348)。唯一指令级差异：ORIG 将 m_4 清零置于数组构造前。
    m_354 = 0;
    m_355 = 0;
    memset(m_infos, 0, sizeof(m_infos));
