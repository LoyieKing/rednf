void StageControl::onEplp(StageEndType::T endType)
{
    // ORIG 0x813064a：bool 岛 -0x9（预清 movb $0x0、case 叶块物化 movb $0/$1、无读者——
    // 4.1 保留死 store；volatile 强制 4.4 保留写与槽）。
    volatile bool flag = false;
    switch ((int)endType)
    {
    case 0:
    case 1:
    case 2:
        flag = false;
        break;
    case 3:
        flag = true;
        break;
    }
    sendEplp(endType);
    Timer_StageControl::RegistTimerEPLPReturnVillage(getIndex());
