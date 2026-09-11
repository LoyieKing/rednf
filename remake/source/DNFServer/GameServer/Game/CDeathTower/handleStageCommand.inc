bool CDeathTower::handleStageCommand(CUser* user, unsigned char cmd)
{
    if (m_party->getManager() != user)
    {
        return 0;
    }
    // ORIG 0x846617d：switch 下探（movzbl -0xc;cmp$1 je;cmp$2 je;jmp default@84661f8）。
    switch (cmd)
    {
    case 1:
        if ((int)!m_stage.setState((ENUM_STAGE_STATE)1))  // ORIG 0x84661a3：xor $0x1,%eax; test; je
        {
            return 1;
        }
        if (_checkMemberDie())
        {
            return 1;
        }
        _onStartStage();
        break;
    case 2:
        if ((int)!m_stage.setState((ENUM_STAGE_STATE)2))  // ORIG 0x84661ea
        {
            return 1;
        }
        break;
    default:
        return 0;  // ORIG 0x84661f8
    }
    return 1;
}