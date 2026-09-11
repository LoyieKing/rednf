void GrowthEquipEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x24, 0);
    tr("GrowthEquipEvent Event end!");
}