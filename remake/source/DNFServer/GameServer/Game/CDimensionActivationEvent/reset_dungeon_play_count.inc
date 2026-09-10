void CDimensionActivationEvent::reset_dungeon_play_count()
{
    // ORIG 0x080eec58：reset_dimensionInout() → trace。
    G_CDataManager()->reset_dimensionInout();
    cMyTrace tr(__PRETTY_FUNCTION__, 0x34, 0);
    tr("CDimensionActivationEvent reset_dungeon_play_count");
