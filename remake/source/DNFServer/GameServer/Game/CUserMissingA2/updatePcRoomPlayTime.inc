void CUser::updatePcRoomPlayTime()
{
    EventClassify::CPcRoomPlayTime* entity =
        static_cast<EventClassify::CPcRoomPlayTime*>(
            static_cast<EventClassify::CEventScriptMng*>(
                G_CDataManager()->get_event_script_mng())
            ->get_event_entity(0x5c));
    if (entity == 0)
        return;
    if (entity->verifyUserPcRoomPlayEvent(this) == 0)
    {
        collectPcRoomPlayTime();
        DB_SavePcRoomPlayTimeEvent::makeRequest(
            get_acc_id(), (unsigned int)getTotalPcRoomPlayTime(),
            getPcRoomPlayTimeRewardCnt());
    }
}