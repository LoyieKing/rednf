void CUser::_onQuestClear(int questId, int /*opt*/)
{
    // ORIG：G_CDataManager()->get_event_script_mng() 在 is_eventing 判与
    // process_add_quest_count_db 两处各自重调（禁缓存，P183/P48）。
    if (static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())->is_eventing(0x193))
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())
            ->process_add_quest_count_db(this, questId);

    // [b28d-22 修法 P197④/P48] ORIG 0x0866445f-0x08664489（及 0x96f 段）为复合 `&&`
    // 短路（`cmpl $0x987; jne; call getCurSec; cmp $0x47b35a50,%eax; jg`，探针
    // /tmp/p22_g1b.cpp qc2 逐条同构）；嵌套 if 反产 setle 物化。
    if (questId == 0x987 &&
        GlobalData::s_systemTime_.getCurSec() <= 0x47b35a50)
        DB_InsertArchieveEventLog::makeRequest(get_acc_id(), getCurCharacNo(),
                                               (SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE)0x32);
    if (questId == 0x96f &&
        GlobalData::s_systemTime_.getCurSec() <= 0x478e7050)
        DB_UpdateHappyBeanInfo::makeRequest(GetUID(), get_acc_id(), getCurCharacNo(),
                                            getCurCharacName());
    // ORIG 0x08664532-0x08664559：GetRepeatEvent(0x13) 后直接虚调（无 ev 局部、无判空）。
    if (questId == 0x970 &&
        GlobalData::s_event_manager->GetRepeatEvent(0x13)->IsEventing((CUser*)0))
        DB_Insert0712WinterEventCoupon::makeRequest(get_acc_id(), getCurCharacNo(), 1);
