// ----------------------------------------------------------------------------
// ORIG T 0x0867925a _ZN5CUser20processGrowTypeEventEhh21eChangeGrowTypeReason：
// reason==1 忽略；转 _onChangeGrowType；CEventScriptMng::process_level_up_reward
// (this,true)；p2==0 时 processLevelUpEventReward(true)。
// ----------------------------------------------------------------------------
void CUser::processGrowTypeEvent(unsigned char p1, unsigned char p2,
                                 eChangeGrowTypeReason reason)
{
    if (reason == (eChangeGrowTypeReason)1)
        return;
    _onChangeGrowType(p1, p2, reason);
    static_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng())
        ->process_level_up_reward(this, true);
    if (p2 == 0)
        processLevelUpEventReward(true);
}