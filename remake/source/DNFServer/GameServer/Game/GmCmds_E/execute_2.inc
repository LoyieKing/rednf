// ORIG execute 0x084b1f28：向监控代理发送 LimitNpcBuyItemRequestInfo
//（type 0x27d8 / len 10）请求限购信息重载。
void NpcLimitBuyItem::execute()
{
    CUser* user = GetUser();
    // b28d-45 P223 变体：ORIG 0x084b1f85 体尾 `jmp; nop` 岛 ⇒ 裸早返形复现。
    if (user == 0)
    {
        return;
    }
    LimitNpcBuyItemRequestInfo req;
    // b28d-45 P0：ORIG 0x084b1f84 无 group/proxy 具名局部（值经寄存器直传，
    // req 槽 -0x16），表达式内联。
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&req), 10);
}