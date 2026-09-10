void CSetTutorialFlagCmd::execute()
{
    double framePad;                                    // P190：对齐撑帧，无读写
    (void)&framePad;
    register unsigned int flag = m_flag;
    GetUser()->RewardTutorial(flag);
