void StageControl::onTimer(TIMER_MESSAGE msg, StageEndType::T endType)
{
    // [b28d-solo-StageControl] P251：ORIG 值全程 %eax 无槽 + cmp 链 ⇒ 原码是
    // switch((int)msg)（窄枚举需 (int) 规避 P305 越界 case 静默删除）；叶块顺序 =
    // case 书写顺序（P46）照 ORIG：a8 → a9 → aa。
    switch ((int)msg)
    {
    case 0xa8:
        onClearReward(endType);
        break;
    case 0xa9:
        onEplp(endType);
        break;
    case 0xaa:
        onEplpReturnVillage();
        break;
    default:
        break;
    }
