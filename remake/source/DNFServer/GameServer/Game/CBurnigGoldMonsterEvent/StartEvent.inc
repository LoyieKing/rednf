// ORIG 0x08116f50：SetEventFlag(true) + cMyTrace(PRETTY, 0x12, 0)
//   ("CBurnigGoldMonsterEvent Event Start!")。
void CBurnigGoldMonsterEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x12, 0);
    trace("CBurnigGoldMonsterEvent Event Start!");
}