// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x08659490：p167=CLEAR，gap=5；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08659490：N82 封顶——帧/槽差 + x87（p167 CLEAR，gap 5）。
void CUser::_processCharacterDayEvent(unsigned int oldFatigue, unsigned int addFatigue)
{
    unsigned int total = oldFatigue + addFatigue;
    if (!(oldFatigue != 0 && total != 0 && oldFatigue < 0x9du))
        return;

    // [K7 2026-09-11 合并旋钮 A+B] A：消「同日跨周」链首重比 dayA==dayB 的 3 条
    //（ORIG 0x086595c2 `je +0x41b` 早返 → 0x086595c8 直落 itemId/addCount 赋值）；
    // B：itemId/addCount 置顶并 `= 0`（ORIG 0x086594c8 `movl $0x0,-0x24(%ebp)` /
    // 0x086594cf `movl $0x0,-0x20(%ebp)`），使 dayA/dayB 临时槽落回 -0x1c/-0x18。
    // 旧 H6 记录「单独 B」234→236 反劣：A 未同消时槽图仍错 ⇒ 本次 A+B 同落。
    unsigned int itemId = 0; int addCount = 0;   // ORIG 0x086594c8/cf（顶部零写）
    // [W16ASymNameSeg2 2026-09-11 符号名轴] ORIG 该处为 .rodata 字面量（0x08cf3960 = 30.0f）、
    // 无函数内 static 实体 ⇒ 内联字面量，消 _ZZN5CUser25_processCharacterDayEventEjjE7DIV_DAY。
    int dayA = (int)(oldFatigue / 30.0f);
    int dayB = (int)(total / 30.0f);
    char body[256];
    if (dayA == dayB)                       // 同日跨周
    {
        // [W16ASymNameSeg2 2026-09-11 符号名轴] ORIG 为 .rodata 字面量（0x08cf3964 = 156.0f）⇒
        // 内联，消 _ZZN5CUser25_processCharacterDayEventEjjE8DIV_WEEK（static const float 即使
        // 只定义不用，GCC 4.4 仍发射符号 ⇒ 必须删定义行）。
        int weekA = (int)(oldFatigue / 156.0f);
        int weekB = (int)(total / 156.0f);
        if (weekA == weekB)
            return;
        itemId = 0x1d5e; addCount = 1;
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_75", 0), 0xff);
    }
    else if (dayA == 0 && dayB == 1)
    {
        itemId = 0x1d5c; addCount = 2;
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_71", 0), 0xff);
    }
    else if (dayA == 1 && dayB == 2)
    {
        itemId = 0x1d5c; addCount = 4;
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_72", 0), 0xff);
    }
    else if (dayA == 2 && dayB == 3)
    {
        itemId = 0x1d5c; addCount = 6;
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_73", 0), 0xff);
    }
    else if (dayA == 3 && dayB == 4)
    {
        itemId = 0x1d5d; addCount = 1;
        strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_74", 0), 0xff);
        CUserEventUtil::updateCharacterDayEvent(this, 1);
    }
    // [M8COB 2026-09-11 语义修正·去发明 return] ORIG **无 `else return`**：
    //   0x0865972d `cmpl $0x3,-0x1c` / 0x08659733 `cmpl $0x4,-0x18` 两条失败的
    //   `jne 0x8659798 <+0x308>` 与成功分支体尾的 `jmp 0x8659798 <+0x308>` 同落点；
    //   而 0x08659798(+0x308) 即 title 段入口（`movl $0x8ceed40,0x8(%esp)` =
    //   "game_server_msg_70" 串地址 + `findString`）。⇒ 全链不匹配时 itemId 保持 0
    //   直落 title 段与 `find_item(0)`（0x086597de 读 -0x24 槽）。
    //   旧稿的 `else return` 凭空造出第 5 个返回出口（尾部多 1 条 `jmp <T>`：ORIG 5 条
    //   = 主体尾 + guard×3 + weekA，我方 6 条）⇒ 按 ORIG 删除。

    char title[21];
    strncpy(title, g_scriptStringManager_.findString(4, "game_server_msg_70", 0), 0x14);
    Inven_Item gift;
    // [P0 去缓存] ORIG 086597de `mov -0x24(%ebp),%ebx; call G_CDataManager;
    // mov %ebx,0x4(%esp); mov %eax,(%esp); call find_item` —— 无 dm 具名指针槽
    // ⇒ 旧稿 -0x18 槽使 body/gift/title 各深 4B（-0x17e/-0x7e/-0x41 vs ORIG -0x17a/-0x7a/-0x3d）。
    CItem* proto = G_CDataManager()->find_item(itemId);
    // [H6B 2026-09-11 第二轮试形·正向包裹] 旧 `if (proto == 0) return;` 产 1 条
    // `jmp epilogue`；ORIG 尾部仅 5 条 jmp（我方 7 条）⇒ 改正向包裹使失败路径
    // fall-through 到 epilogue（省 1 条 jmp）。
    if (proto != 0)
    {
        gift.m_addInfo = itemId;
        proto->make_item(gift);                 // virtual vtable+0x08 槽
        gift.set_add_info(addCount);
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            title, gift, 0, getCurCharacNo(), body, strlen(body), 0, GetServerGroup(), 0, 0);
    }
    // [M8COB 2026-09-11] ORIG 0x086598a0（主体尾，`call ReqDBSendNewSystemMail` 后）
    // = `jmp 0x86598af <+0x41f>`（越过紧随其后的 4 个 return 出口块 0x412/415/418/41b），
    // 显式尾 `return` 复现该 jmp（旧稿无尾 return ⇒ 主体 fallthrough 进出口块，少 1 条 jmp）。
    return;
}