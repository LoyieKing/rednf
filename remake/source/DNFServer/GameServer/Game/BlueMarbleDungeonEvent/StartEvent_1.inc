// ORIG 0x080dc52c：SetEventFlag(true) + cMyTrace(__FUNCTION__, 0x1c, 0)
//   ("BlueMarbleDungeonEvent Start!")。完整实现，非 thunk。
void BlueMarbleDungeonEvent::StartEvent(Word_Param /*param*/)
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x1c, 0);
    trace("BlueMarbleDungeonEvent Start!");
}