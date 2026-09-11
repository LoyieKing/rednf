// ORIG 0x0811bac2：SetEventFlag(true) + cMyTrace(PRETTY, 0xe, 0)
//   ("Assault On Off Event Start!")。
void CAssaultOnOffEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace trace(__PRETTY_FUNCTION__, 0xe, 0);
    trace("Assault On Off Event Start!");
}