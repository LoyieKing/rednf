// ---------------------------------------------------------------------------
// ORIG 0x08268d86：SetEventFlag(true) +
// cMyTrace(__FUNCTION__,0x12,0)("CPvPExpPenaltyEvent Start!")。
// __FUNCTION__ 生成局部静态 _ZZN19CPvPExpPenaltyEvent10StartEventEvE12__FUNCTION__
// （.rodata 0x08bf3f1b 内容 "StartEvent"），勿用手写字面量。
void CPvPExpPenaltyEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x12, 0);
    tr("CPvPExpPenaltyEvent Start!");
}