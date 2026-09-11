// ----------------------------------------------------------------------------
// ORIG T 0x0812fbfa（onTimer.md 全文）：msg 0xa9 → onEplp(endType)；
//   0xaa → onEplpReturnVillage()；0xa8 → onClearReward(endType)；其余无操作。
//   TIMER_MESSAGE 0xa8-0xaa 枚举器名待 TimerDispatcher 批次定名，暂用字面值。
// ----------------------------------------------------------------------------
// [b28d-D9] P85/P209：ORIG 线性比较链值全程 %eax；4.4 signed 形参 mem 直比（unsigned 强转实测反劣 29>27）（ORIG 0x0812fbfa sz 0x00000053）
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
}