// ----------------------------------------------------------------------------
// ORIG T 0x086792d6 _ZN5CUser19processAwakenRewardEc：
// switch(stage) 设定邮件正文/标题脚本文案（jump table），命中后构造
// Inven_Item(0x3c0, sealCount=1) 并 ReqDBSendNewSystemMail；default 直接返回。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N7+P52：ORIG 0x086792d6；虚调用 callee 寄存器 %eax↔%edx 互换
// （4.4 间接 call 目标恒 %eax）+ 值先物化 callee-saved 缺失；p167 CLEAR（32/32），
// 语义闭合。
// [b28d-solo-CUserMissingA2] 修复 P290：rewardItemId int→unsigned ⇒ find_item
// 实参升格 %ebx 跨 G_CDataManager call 保活（ORIG 0x08679609 对齐）；残余仅 N7
// （本 TU 路由 4.4.7 ⇒ 间接 call 目标恒 %eax），201/201。
void CUser::processAwakenReward(char stage)
{
    unsigned int rewardItemId = 0x3c0;
    int sealCount = 1;
    char mailMsg[256];
    char mailTitle[21];
    switch (stage)
    {
    case 0:
        strncpy(mailMsg, g_scriptStringManager_.findString(
                    4, "awaken_mail_msg_1", 0), 0xff);
        strncpy(mailTitle, g_scriptStringManager_.findString(
                    4, "awaken_mail_title_1", 0), 0x14);
        break;
    case 1:
    case 7:
        strncpy(mailMsg, g_scriptStringManager_.findString(
                    4, "awaken_mail_msg_2", 0), 0xff);
        strncpy(mailTitle, g_scriptStringManager_.findString(
                    4, "awaken_mail_title_2", 0), 0x14);
        break;
    case 2:
    case 5:
        strncpy(mailMsg, g_scriptStringManager_.findString(
                    4, "awaken_mail_msg_3", 0), 0xff);
        strncpy(mailTitle, g_scriptStringManager_.findString(
                    4, "awaken_mail_title_3", 0), 0x14);
        break;
    case 3:
    case 8:
        strncpy(mailMsg, g_scriptStringManager_.findString(
                    4, "awaken_mail_msg_4", 0), 0xff);
        strncpy(mailTitle, g_scriptStringManager_.findString(
                    4, "awaken_mail_title_4", 0), 0x14);
        break;
    case 4:
        strncpy(mailMsg, g_scriptStringManager_.findString(
                    4, "awaken_mail_msg_5", 0), 0xff);
        strncpy(mailTitle, g_scriptStringManager_.findString(
                    4, "awaken_mail_title_5", 0), 0x14);
        break;
    case 6:
        strncpy(mailMsg, g_scriptStringManager_.findString(
                    4, "awaken_mail_msg_6", 0), 0xff);
        strncpy(mailTitle, g_scriptStringManager_.findString(
                    4, "awaken_mail_title_6", 0), 0x14);
        break;
    default:
        return;
    }

    Inven_Item item;
    CItem* def = G_CDataManager()->find_item(rewardItemId);
    if (def == 0)
        return;
    item.m_addInfo = rewardItemId;
    def->make_item(item);
    item.set_add_info(sealCount);
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        mailTitle, item, 0, getCurCharacNo(), mailMsg, (int)strlen(mailMsg),
        12, GetServerGroup(), false, false);
}