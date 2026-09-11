// ORIG 0x08109e26：SetEventFlag(true) + cMyTrace(__FUNCTION__, 0x14, 0)
//   ("CBreakAwayPreventEvent Start!")。__FUNCTION__ 名（0x0b 长度符号）。
void CBreakAwayPreventEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x14, 0);
    trace("CBreakAwayPreventEvent Start!");
}