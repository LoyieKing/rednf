// ---- send_lose_item_for_china @ 0x0852e3a8 ----
// [b28d-solo-CTradeSpace verdict=IDENTICAL/AE ndiff=0（O110/N110）]
//   修法：①if 判空改正向包裹（ORIG je 到尾部 return 0 岛，P222/P285）；
//   ②msg(0xff)/title(29) 先/后声明使 4.4 槽位与 ORIG 同址（msg -0x134 4 对齐
//   → 简单 rep stos；title -0x35 非 4 对齐 → 对齐剥离慢路，P312）；
//   ③findString/strlen/getCurCharacNo/GetServerGroup 全部内联实参（P0/P86），
//   消 5 个具名局部槽（帧 0x16c→0x15c）。
int CTradeSpace::send_lose_item_for_china(CUser* user, Inven_Item& item)
{
    // b28d-solo-CTradeSpace：ORIG 0x0852e3a8。
    //   title/msg 的清零在 item 判空之后 ⇒ 原码是 if 后的 `char x[N] = {0};`
    //   （4.1 对 memset 内联展开；4.4 对 29B 直写、255B 走对齐剥离慢路，N78 残差）。
    //   槽位：msg(0xff) 先声明占深槽 -0x134（4 对齐 → 简单 rep stos），
    //   title(29B) 后声明占浅槽 -0x35（非 4 对齐 → 慢路），与 ORIG 同址。
    if (item.m_addInfo != 0) {
        char title[29] = {0};
        char msg[0xff] = {0};
        strncpy(title, g_scriptStringManager_.findString(4, "game_server_msg_287", 0), 0x1d);
        strncpy(msg, g_scriptStringManager_.findString(4, "game_server_msg_288", 0), 0xff);
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            title, item, 0, (unsigned int)user->getCurCharacNo(), msg, (int)strlen(msg), 0,
            user->GetServerGroup(), false, false);
        return 1;
    }
    return 0;
}