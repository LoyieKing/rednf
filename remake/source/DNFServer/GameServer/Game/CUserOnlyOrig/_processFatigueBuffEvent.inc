void CUser::_processFatigueBuffEvent(unsigned int oldFatigue, unsigned int addFatigue)
{
    unsigned int total = oldFatigue + addFatigue;
    if (!(oldFatigue != 0 && total != 0 && oldFatigue < 0x9du))
        return;
    static const float DIV = 50.0f;  // ORIG .rodata 常量（0x08cf3968 处值 50.0，已验证）
    // [b28d-22 修法 P211/P218] itemId int→unsigned int：ORIG 0x08659afe `mov -0x1c(%ebp),%ebx;
    // call G_CDataManager; mov %ebx,0x4(%esp)`（实参先升格 callee-saved 跨 receiver call 保活）。
    // 4.4 对 int 局部恒反序（探针 /tmp/p22_arg2.cpp k/l：call 后才 `mov -0xc,%edx`），
    // unsigned 局部（/tmp/p22_arg3.cpp o）逐条复现 ORIG 形。
    unsigned int itemId = 0;      // ORIG 顶部清零（movl $0x0,-0x1c）
    int addCount = 0;    // ORIG 顶部清零（movl $0x0,-0x18）
    int zoneA = (int)(oldFatigue / DIV);
    int zoneB = (int)(total / DIV);
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
