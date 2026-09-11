// ---------------------------------------------------------------------------
// ORIG 0x0811785c：thunk —— 经 vptr slot2 虚调 StartEvent()（mov (%eax),%eax;
// add $0x8; mov (%eax),%edx; call *%edx），不读 param。
void CStopOverlabExpEvent::StartEvent(Word_Param param)
{
    StartEvent();
}