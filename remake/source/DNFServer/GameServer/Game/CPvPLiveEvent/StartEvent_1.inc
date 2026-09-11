// ---------------------------------------------------------------------------
// ORIG 0x08268f74：与 StartEvent() 同体（SetEventFlag(true) + trace，
// __FUNCTION__ 局部静态 _ZZN13CPvPLiveEvent10StartEventE10Word_ParamE12__FUNCTION__
// @0x08bf4650，行 0x19）。param 未被使用（ORIG 亦未引用形参）。
void CPvPLiveEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x19, 0);
    tr("CPvPLiveEvent Start!");
}