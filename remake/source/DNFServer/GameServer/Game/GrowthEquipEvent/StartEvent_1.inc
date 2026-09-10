void GrowthEquipEvent::StartEvent(Word_Param /*param*/)
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1e, 0);
    tr("GrowthEquipEvent Event start! [monitor request]");
