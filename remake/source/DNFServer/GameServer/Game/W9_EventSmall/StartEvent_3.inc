void CFatigueBuffEvent::StartEvent(Word_Param)  // ORIG 0x0811bca6：虚调用 slot2（StartEvent()）
    // [b28c-27] AE（fresh 复验）：共享头改动后本件 .o 与 ORIG AE 全等
    // （虚调用目标寄存器形在 W9_EventSmall 路由下与 ORIG 一致）。
{
    StartEvent();
}