void GrowthEquipEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x18, 0);
    tr("GrowthEquipEvent Event start!");
