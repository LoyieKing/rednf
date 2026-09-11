// ORIG 0x080dc57e：SetEventFlag(false) + cMyTrace(__FUNCTION__, 0x22, 0)
//   ("BlueMarbleDungeonEvent End!")。
void BlueMarbleDungeonEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__FUNCTION__, 0x22, 0);
    trace("BlueMarbleDungeonEvent End!");
}