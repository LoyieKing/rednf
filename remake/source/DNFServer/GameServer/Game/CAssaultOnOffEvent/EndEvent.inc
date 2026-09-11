// ORIG 0x0811bb2e：SetEventFlag(false) + cMyTrace(PRETTY, 0x18, 0)
//   ("Assault On Off Event End!")。
void CAssaultOnOffEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x18, 0);
    trace("Assault On Off Event End!");
}