// >>> W7-BLOCKED CUser::EnableCharacInfo —— 残余混合依赖，详见 /tmp/w7_blocked.tsv 汇总
// ---------------------------------------------------------------------------
// 25. CUser::EnableCharacInfo(CACHE_SECTION) W 0x081253f4【报告缺失·objdump 0x81253f4】
// ORIG: movb $0x1, this+0x8d0fd+section
// void CUser::EnableCharacInfo(CACHE_SECTION section)
// {
//     m_pad8d0fd[(int)section] = 1;
// }
//
// 26. CUser::eventDeliveryQuestClear(int,int,vector<pair<int,int>>&,int,int,int) T 0x086645a4
// QuestClearDeliveryMsg 已建模（QuestClearDeliveryMsg.h；ctor ORIG W 0x08697914）。
QuestClearDeliveryMsg::QuestClearDeliveryMsg()
{
    m_questParamA = 0;       // +0x10
    m_paramD = 0;            // +0x14
    m_paramE = 0;            // +0x18
    m_paramF = 0;            // +0x1c
    m_itemsPtr = 0;          // +0x20
}