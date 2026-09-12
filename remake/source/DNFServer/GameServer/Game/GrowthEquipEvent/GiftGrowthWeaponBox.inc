// ============================================================================
// GrowthEquipEvent::GiftGrowthWeaponBox @ 0x080f7050（const）
// 活动礼包箱领取（每角色一次，data+0x00 状态字节守卫）：按脚本配置的礼包箱
// idx（m_growthWeapon.m_giftBox，ORIG CDataManager+0x4bc4）构造 Inven_Item，
// 经 RDAR 字符串 game_server_msg_277/278 走系统邮件，再发 DB 存档请求并刷新
// 邮件提醒 tooltip。
// ============================================================================
void GrowthEquipEvent::GiftGrowthWeaponBox(CUser& user) const
{
    // [实测] ORIG 0x080f70xx `mov %eax,-0x18(%ebp)` + `mov -0x18(%ebp),%eax;
    // movzbl (%eax)` ⇒ data **是具名局部并占槽**（ORIG 的 `push %esi` 属 characNo，
    // 见下）；`register` 形（用 %ebx）实测反劣 ⇒ 保持具名。
    GrowthWeaponEvent* data =
        static_cast<GrowthWeaponEvent*>(user.getGrowthWeaponEventdata());
    // [R14a07] P357 字段轴 + P59 正向包裹：ORIG 0x080f7069 `movzbl (%eax),%eax;
    // xor $0x1,%eax; test %al,%al; je 0x080f721c`（bool 字段 + `!field`，je 直落
    // 共享尾、无 nop 中继岛）⇒ 原码为「if (!field) { 发放 }」正向包裹，非早退
    // guard；字段为 CUser.h:1984 `bool m_eventFlag @+0x00`（同一 xor 判据）。
    if (!data->m_eventFlag)
    {
    data->m_eventFlag = true;

    const int giftBoxIdx = G_CDataManager()->m_growthWeapon.m_giftBox;

    Inven_Item item;                // ORIG Inven_Item::Inven_Item() 0x080cb854
    CItem* itemBase = G_CDataManager()->find_item(giftBoxIdx);

    // [R21-07 复刻修复] ORIG 0x080f70b0 `movl $0x0,-0xc(%ebp)`：serverGroup 有
    // 初值 0（位于 find_item 之后、G_GameWorld 之前），旧形 `int serverGroup;`
    // 缺该清零 ⇒ 补 `= 0`（两分支随后各赋值 ⇒ 清零不可观察，形态复刻）。
    int serverGroup = 0;
    // [R20a07 §26.3/P354] ORIG 0x080f70c4 `test %al,%al; je 0x080f70d8`（**无** xor；
    // 原值 true ⇒ fall-through 走 GetServerGroup，false ⇒ 跳后置的 env 块）；
    // 我方旧形 `if (!f()) {env} else {GetServerGroup}` 使 4.4 产 `xor $1; test; je`
    // 且两臂块序相反 ⇒ 改正向 if/else 复现 ORIG（消多余物化 + 块序翻正）。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        serverGroup = (int)user.GetServerGroup();
    else
        // [P6S3 round6 · 父泳道线索（p167fix EXTRA `CEnvironment::get_server_group`）+
        // 同 TU 既有判例] 本件 ORIG 与 SendGrowthWeaponEventInfo 同形：**直读字段**
        // `call G_CEnvironment; mov 0x378(%eax),%eax`（= m_serverEnvir.m_serverGroup，
        // +0x198+0x1e0），不发 `get_server_group()` call（该 getter 我方仅 weak 定义、
        // 语义等价于返回同一字段 ⇒ 直读为 ORIG 形）。
        serverGroup = G_CEnvironment()->m_serverEnvir.m_serverGroup;  // ORIG +0x378

    if (itemBase == 0)
        return;

    // [R6a20 ④ 修复] ORIG 0x080f70f0 `mov -0x14(%ebp),%eax; mov %eax,-0x53(%ebp)`：
    //   -0x53 = item+0x02 = Inven_Item::m_addInfo（pack(1) 布局见 Inven_Item.h），
    //   即源码在此把礼包箱 idx 写进 item 本体；我方此前漏写该字段。
    item.m_addInfo = giftBoxIdx;

    // vtable+0x08 = CItem::make_item(Inven_Item&) const
    itemBase->make_item(item);
    item.set_add_info(1);           // ORIG Inven_Item::set_add_info(item,1)

    // 邮件正文：RDAR 字符串 game_server_msg_278（strncpy 0xff 忠实保留）。
    char title[256];
    strncpy(title,
            g_scriptStringManager_.findString(
                4, "game_server_msg_278", static_cast<bool*>(0)),
            0xff);

    // [R6a20 ④ 修复] ORIG 0x080f7160 `mov -0x53(%ebp),%eax; test %eax,%eax; je`：
    //   判定读的是 item.m_addInfo（同槽），非本地 giftBoxIdx ⇒ 复刻 ORIG 读源。
    if (item.m_addInfo != 0)
    {
        // [b28c-L18/N2 复核] ORIG 0x080f7055 `sub $0x180,%esp` + `push %esi;push %ebx`：
        // len/characNo 均无槽（%esi/%ebx 保活）。实测去具名局部三个变体全部反劣：
        // 去两者 115/118、只去 characNo 115/117、只去 len 115/117（基线 115/116）
        // ⇒ 4.4 在实参表达式形下把 findString 的 this/串实参多物化，保持具名局部。
        const int titleLen = static_cast<int>(strlen(title));
        const unsigned int characNo =
            static_cast<unsigned int>(user.getCurCharacNo());
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            g_scriptStringManager_.findString(
                4, "game_server_msg_277", static_cast<bool*>(0)),
            item, 0u, characNo, title, titleLen, 0u,
            static_cast<ENUM_SERVER_GROUP>(serverGroup), false, false);
    }

    DB_EventSaveGrowthWeapon::makeRequest(user.get_acc_id(),
                                          static_cast<short>(serverGroup));
    user.SendEventMailAlaramTooltip();
    }
}