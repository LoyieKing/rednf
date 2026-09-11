// ORIG 0x08116fbc：SetEventFlag(false) + cMyTrace(PRETTY, 0x1c, 0)
//   ("CBurnigGoldMonsterEvent Event End!")。
void CBurnigGoldMonsterEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x1c, 0);
    trace("CBurnigGoldMonsterEvent Event End!");
}