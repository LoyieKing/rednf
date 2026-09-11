// ORIG 0x080dc4da：SetEventFlag(true) + cMyTrace(__FUNCTION__, 0x16, 0)
//   ("BlueMarbleDungeonEvent Start!")。__FUNCTION__ 名（0x0b 长度符号）。
void BlueMarbleDungeonEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x16, 0);
    trace("BlueMarbleDungeonEvent Start!");
}