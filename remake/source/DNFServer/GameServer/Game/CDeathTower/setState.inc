bool CDeathTower::CStage::setState(ENUM_STAGE_STATE state)
{
    // ORIG 0x8461c5d：switch 下探形态（范式34）：mov 0xc,%eax; cmp$1 je;cmp$2 je;jmp default。
    switch (state)
    {
    case 1:  // STAGE_READY?（枚举值 1）：非 0 拒绝（ORIG 0x8461c6c）
        if (m_state != 0)
        {
            return false;
        }
        break;
    case 2:  // 非 1 拒绝（ORIG 0x8461c80）
        if (m_state != 1)
        {
            return false;
        }
        break;
    default:
        return false;  // ORIG 0x8461c95
    }
    m_state = state;
    return true;
