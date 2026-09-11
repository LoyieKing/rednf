void COnTimeEvent::StartEvent(Word_Param param)
{
    (void)param;
    // [b28c-lane17] 判词：22=22 助记符全等，唯一残差 = 下方虚调用的目标寄存器
    // （ORIG `mov (%eax),%edx; …; call *%edx` vs 4.4 恒 `call *%eax`）⇒ N7
    // （4.4 间接 call 目标恒 %eax，范式库负例表），无源形可翻。
    this->StartEvent();   // ORIG @0x081bb8ca：虚调 slot+0x8（本类 StartEvent）
    cMyTrace trace(__PRETTY_FUNCTION__, 0x47, 0);
    trace("COnTimeEvent Event Start!");
}