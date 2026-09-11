// ---------------------------------------------------------------------------
// 12) CUser::_processFatigueBuffEvent(uint, uint) —— 同型（DIV=.rodata, 1/2/3→0x1d95）
//     报告：docs/class_func_reports/CUser/_processFatigueBuffEvent.md。ORIG 0x086598ba。
// ---------------------------------------------------------------------------
void CUser::_processFatigueBuffEvent(unsigned int oldFatigue, unsigned int addFatigue)
{
    unsigned int total = oldFatigue + addFatigue;
    if (!(oldFatigue != 0 && total != 0 && oldFatigue < 0x9du))
        return;
    // [W16ASymNameSeg2 2026-09-11 符号名轴] ORIG 为 .rodata 字面量（0x08cf3968 = 50.0f）⇒
    // 内联，消 _ZZN5CUser24_processFatigueBuffEventEjjE3DIV。
    // [b28d-22 修法 P211/P218] itemId int→unsigned int：ORIG 0x08659afe `mov -0x1c(%ebp),%ebx;
    // call G_CDataManager; mov %ebx,0x4(%esp)`（实参先升格 callee-saved 跨 receiver call 保活）。
    // 4.4 对 int 局部恒反序（探针 /tmp/p22_arg2.cpp k/l：call 后才 `mov -0xc,%edx`），
    // unsigned 局部（/tmp/p22_arg3.cpp o）逐条复现 ORIG 形。
    unsigned int itemId = 0;      // ORIG 顶部清零（movl $0x0,-0x1c）
    int addCount = 0;    // ORIG 顶部清零（movl $0x0,-0x18）
    int zoneA = (int)(oldFatigue / 50.0f);
    int zoneB = (int)(total / 50.0f);
    if (zoneA == zoneB) return;
    itemId = 0x1d95;
    char body[256];
    // ORIG：findString 调用点逐分支独立（每分支一 findString+strncpy），
    // 不合并入 key 变量——访问器/查表结果禁缓存（战术 P183/P48）。
    if (zoneA == 1 && zoneB == 2) {
        addCount = 10;
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_84", 0), 0xff);
    }
    else if (zoneA == 2 && zoneB == 3) {
        addCount = 20;
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_85", 0), 0xff);
    }
    else return;
    char title[21];
    strncpy(title, g_scriptStringManager_.findString(4, "game_server_msg_83", 0), 0x14);
    Inven_Item gift;
    CItem* proto = G_CDataManager()->find_item(itemId);
    if (!proto) return;
    gift.m_addInfo = itemId;
    proto->make_item(gift);
    gift.set_add_info(addCount);
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        title, gift, 0, getCurCharacNo(), body, strlen(body), 0, GetServerGroup(), 0, 0);
}