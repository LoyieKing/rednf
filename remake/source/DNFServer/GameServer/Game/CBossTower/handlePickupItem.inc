int CBossTower::handlePickupItem(CUser* user, int a, bool b, char c)
{
    // [b28d-solo-CBossTower 真语义 bug 修正] ORIG 0x08143d3e..0x08143d68：
    //   m_party==0 → `je` 直达 0x08143d5a `mov $1,%eax`（**不**调 IsRoutingState）；
    //   m_party!=0 时 `call IsRoutingState; test %al,%al; je <mov $0 岛>`；
    //   物化岛 mov$1/jmp/mov$0 + `test %al,%al; je <正常路径>` ⇒ 错误路径条件 =
    //   `(m_party==0) || IsRoutingState()`（负极性短路，P214①）。旧重建写
    //   `m_party != 0 && IsRoutingState()` —— m_party==0 时误入正常路径（真 bug）。
    if (m_party == 0 || m_party->IsRoutingState())
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x2e), 0xd8);
        return 0;
    }

    // ORIG 0x08143d8f `movb $0x0,-0x1d` / 0x08143dbb `mov %al,-0x9`（**无**
    // test/setne ⇒ callee 单字节返回，P248 档①）。ORIG 槽序：serverRarity
    // -0x1d（深）在前、picked -0x9（浅）在后（4.1 先声明更深）。
    bool picked;             // ORIG -0x9 槽
    char serverRarity = 0;   // ebp-0x1d
    picked = CBossStage::handlePickupItem(user, a, b, serverRarity);
    if (!picked)
        return 0;

    if (serverRarity != c)
    {
        cMyTrace trace("virtual bool WongWork::CBossTower::handlePickupItem(CUser*, "
                       "int, bool, char)", 600, 5);
        trace("server_item_rarity != client_item_rarity");
    }

    if (user != 0 && c == 3)
    {
        m_dungeonLog.IncrementBossPickUpItem(
            *user,
            static_cast<unsigned char>(m_stageCur),
            static_cast<unsigned char>(m_difficulty));
    }
    return 1;
