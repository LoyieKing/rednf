void CFatigueBuffEvent::StartEvent()         // ORIG 0x0811bc54
{
    SetEventFlag(true);
    // [N3 2026-09-11 §6.5 真错] ORIG prett 实读 = "virtual void CFatigueBuffEvent::StartEvent()"
    //   （0x08b51820 size 0x2d，GCC 对虚函数加 virtual 前缀）；旧形 __FUNCTION__ 只展开裸
    //   函数名 "StartEvent" ⇒ 运行期日志文本不同（verdict 恒不变不可见）⇒ 改 __PRETTY_FUNCTION__。
    cMyTrace trace(__PRETTY_FUNCTION__, 0x11, 0);
    trace("CFatigueBuffEvent Event Start!");
}