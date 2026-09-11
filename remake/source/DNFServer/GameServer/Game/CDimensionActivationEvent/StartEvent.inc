void CDimensionActivationEvent::StartEvent()
{
    // ORIG 0x080eeaee：SetEventFlag(true) → trace → set_dungeon_play_count()。
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x12, 0);
    tr("CDimensionActivationEvent Event Start!");
    set_dungeon_play_count();
}