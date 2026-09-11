// ORIG 0x0819e7f4（thunk，0x1a）：经 vptr+8 虚调 StartEvent()。
void Arad_MomijiEvent::StartEvent(Word_Param /*param*/)
{
    // [b28c-27] N7 封顶：唯一残差=虚调用目标寄存器（ORIG `mov (%eax),%edx; ...;
    // call *%edx` vs 4.4 `mov (%eax),%eax; ...; call *%eax`）；4.4 间接 call 目标
    // 恒 %eax，源码形不可择（探针 /tmp/l22p/vprobe*.cpp）。
    StartEvent();
}