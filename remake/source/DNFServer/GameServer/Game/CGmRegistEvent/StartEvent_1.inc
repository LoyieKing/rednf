// ---------------------------------------------------------------------------
// ORIG 0x0811cd2c：同 StartEvent()（file 串
// "virtual void CGmRegistEvent::StartEvent(Word_Param)"，行 0x1d）。
void CGmRegistEvent::StartEvent(Word_Param param)
{
    DB_UpdateGMData::makeRequest(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1d, 0);
    tr("CGmRegistEvent Start!");
}