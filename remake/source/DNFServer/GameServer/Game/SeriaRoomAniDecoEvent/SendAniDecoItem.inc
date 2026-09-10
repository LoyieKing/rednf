void SeriaRoomAniDecoEvent::SendAniDecoItem(CUser& user)  // 0x0827b75a
{
    // [b23S3C] ORIG 0x0827b75a 形核（P48/每用点重取）：①单例 G_CDataManager() 在
    // maxAniDecoItem/getSeriaRoomAniDecoItem/find_item 三用点各重取（无具名缓存槽）；
    // ②getSeriaRoomAniDecoInfo() ORIG call 两次（sret @-0x24 供条件比较、@-0x28 供
    // itemIdx 入参与尾段 ++/set/makeRequest），故写两次独立取用而非复用同一局部；
    // ③sg 去具名局部直落实参（ORIG 直落 0x1c(%esp)，无 -off 往返）；
    // ④strncpy 后 ORIG 无补 NUL 的 movb ⇒ 删 title[255]。
    // 残余：ORIG `movzwl+setl` vs 4.4 `movzwl+setge/jne` 极性与 +2 字段的 cwtl/movswl
    // 单段 sext 选择 = 负例表"跨版本 sext/zext"/setcc-fold；帧 0x1ac vs 0x19c = N82。
    seriaRoom_AniDeco deco = user.getSeriaRoomAniDecoInfo();
    if (deco.m_field0 < static_cast<short>(G_CDataManager()->maxAniDecoItem()))
    {
        seriaRoom_AniDeco deco2 = user.getSeriaRoomAniDecoInfo();
        int itemIdx = G_CDataManager()->getSeriaRoomAniDecoItem(deco2.m_field0);
        // P223 多岛细则：ORIG 守卫2/守卫4 各落独立 nop 中继岛（+0x231/+0x234）
        // ⇒ 负条件 goto + 尾标签串（正向嵌套 if 被 4.4 jump-thread 成 0 岛）。
        if (itemIdx == 0)
            goto L_itemIdx;
        {
            Inven_Item item;
            CItem* src = G_CDataManager()->find_item(itemIdx);
            if (src != 0)
            {
                // ORIG 0x0827b80b `mov %eax,-0x63` = item(@-0x65)+0x02 ⇒ 写字段。
                item.m_addInfo = src->get_index();
                src->make_item(item);
                item.set_add_info(1);                   // 绑定发放

                // 标题取 game_server_msg_290、正文 289（RDAR 表 4）。
                char title[256];
                strncpy(
                    title,
                    g_scriptStringManager_.findString(4, "game_server_msg_290", 0),
                    0xff);
                if (item.m_addInfo == 0)
                    goto L_index;
                {
                    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                        g_scriptStringManager_.findString(
                            4, "game_server_msg_289", 0),
                        item, 0, (unsigned int)user.getCurCharacNo(),
                        title, (int)strlen(title), 0,
                        static_cast<ENUM_SERVER_GROUP>(
                            user.GetServerGroupToPvP()),
                        false, false);

                    deco2.m_field0 = deco2.m_field0 + 1;
                    user.setSeriaRoomAniDecoInfo(deco2);
                    DB_UpdateSeriaRoomAnimationDeco::makeRequest(
                        user.get_acc_id(),
                        (short)user.GetServerGroupToPvP(),
                        user.getCurCharacNo(),
                        deco2.m_field0, deco2.m_field2);
                }
            }
        }
    }
L_itemIdx:;
L_index:;
