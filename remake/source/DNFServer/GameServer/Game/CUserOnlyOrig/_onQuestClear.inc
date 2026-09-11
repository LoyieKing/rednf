// >>> W7-BLOCKED CUser::_onCreateCharacter —— 残余混合依赖，详见 /tmp/w7_blocked.tsv 汇总
// ---------------------------------------------------------------------------
// 8) CUser::_onCreateCharacter(uint accId→playerUid? 按 ORIG param_1=user uid, char* name, char type)
//    报告：docs/class_func_reports/CUser/_onCreateCharacter.md。ORIG 0x0864a432。
// ---------------------------------------------------------------------------
// void CUser::_onCreateCharacter(unsigned int userId, char* name, char createType)
// {
//     (void)name;
//     if (createType == 4)
//     {
//         if (GlobalData::s_systemTime_.getCurSec() < 0x4936f351u)
//         {
//             Inven_Item gift;                                // Inven_Item::Inven_Item()
//             gift.m_addInfo = 0x1d19;
//             CItemList& itemList = *G_CDataManager()->getItemList();
//             itemList.create_item((ENUM_ITEM_CREATE_TYPE)3, gift, 1);
//             if (gift.m_addInfo != -1)
//             {
//                 char title[20];
//                 char body[256];
//                 strncpy(title, g_scriptStringManager_.findString(4, "game_server_msg_56", 0), 0x14);
//                 strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_57", 0), 0xff);
//                 WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
//                     title, gift, 0, userId, body, strlen(body), 0,
//                     (ENUM_SERVER_GROUP)0, false, false);
//             }
//         }
//     }
//     _processUnlimitChallenge(userId, createType);
// }
//
// ---------------------------------------------------------------------------
// 9) CUser::_onQuestClear(int, int questId) —— ORIG T _ZN5CUser13_onQuestClearEii
//    @0x08664412。报告：docs/class_func_reports/CUser/_onQuestClear.md（全文）。
//    0x193 计数事件 → process_add_quest_count_db；0x987/0x96f 按 systemTime
//    门控投 DB；0x970 经 repeat 0x13 事件 IsEventing 门控投 0712 冬季券。
// ---------------------------------------------------------------------------
// [b28d-22 N7/N82] ORIG 0x08664412：p167=CLEAR，gap=0；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig NEAR] ORIG 0x08664412：助记符序列相同；残余为寄存器/栈槽分配跨版本差（P112/N82 族），语义等价。
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
}