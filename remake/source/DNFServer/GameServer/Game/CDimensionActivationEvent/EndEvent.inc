void CDimensionActivationEvent::EndEvent()
{
    // ORIG 0x080eeb64：SetEventFlag(false) → trace → reset_dungeon_play_count()。
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1e, 0);
    tr("CDimensionActivationEvent Event End!");
    reset_dungeon_play_count();
}