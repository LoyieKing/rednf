void CUser::compoundGuildMessage(Inven_Item const& item)
{
    // [b28d-22 修法 P0/P52/P253] ORIG 0x0869305a-0x0869306e 内联 `rep stos`（120B）
    // ⇒ 原码是 `char buf[0x78] = {0};`（显式 memset 恒 call，探针 /tmp/p22_memset.cpp）；
    // 0x0869307b getKey 结果免具名 key 直传；0x0869310b/0x08693118 gkey/cno 落 %esi/%ebx
    // 跨调用保活 ⇒ 尾部四局部删、单语句链式（P52）。
    char buf[0x78] = {0};
    CItem* def = G_CDataManager()->find_item(item.getKey());
    if (def != NULL) {
        strcpy(buf, getCurCharacName());
        sprintf(buf, "%s님이 +%d %s 조합에 성공 하였습니다.",
                getCurCharacName(), (unsigned)item.GetUpgrade() & 0xff, def->GetItemName());
        GlobalData::s_guild_proxy_mgr
            ->GetServerProxy((ENUM_SERVER_GROUP)GetServerGroup())
            ->SendGuildBoardWrite(buf, 0u, getCurCharacNo(), -1, (char)-1,
                                  get_charac_guildkey());
        // [b28d-solo-CUserOnlyOrig P285] ORIG 0x08693169 正常路径尾单条
        // `jmp <epilogue>; nop`：纯正向 if 被 4.4 jump-thread；加空 else{return;}
        // 即保留体尾 jmp+nop 岛（P285 复现形）。
    } else {
        return;
    }
