// [b23S3C] ORIG 0x0827ba52 `movzwl -0x22(%ebp)` = deco(sret @-0x24)+2 ⇒ 奖励索引用
// deco_flow(m_field2)，原码误取 m_field0（+0 是 receive_flow 计数）⇒ 真 bug 修正；
// strncpy 后 ORIG 无 `movb $0x0,title+255` ⇒ 删补 NUL 行（与 SendAniDecoItem 同）
// 残余 28 全为帧槽号平移（4.1 把 index 挤在 item 之后 -0x5f、4.4 独立高槽 = N82）、
// N7 虚调用 %edx/%eax 归属、以及 `mov MEM,%eax; test` vs 4.4 `cmpl $0,MEM`（N-88 候选）
// ⇒ 语义已核，调用点级无剩余缺口。
// [b28d-solo-W8LT_EventTail] DIFF→NEAR(N7) | ORIG 0x0827ba52 | P167 CLEAR（15/15）。
// 修法：`int index = src->get_index()` 改为直接写 `item.m_addInfo`（ORIG
// 0x0827bad5 `mov %eax,-0x5f` = item@-0x61 + 0x02）⇒ 槽集/帧全对齐、102=102；
// 亦解释 ORIG 的 unsigned 字段 test 形（P209）。残余（N 封顶，与 SendAniDecoItem
// 同族）：ReqDBSendNewSystemMail 实参中 `GetServerGroupToPvP()`↔`strlen()` 的
// callee-saved 角色互换——ORIG 4.1 esi→edi→ebx，4.4.6 edi→esi→ebx（N109；本 TU
// 已路由 4.4.6 后端，P230 亦不翻；两值无槽往返，P0 禁具名缓存）。
// [M2S6 2026-09-11 复核] 102=102，残余仍仅 N109 + AE；§6.5 串核：
// game_server_msg_291（我方 .rodata 0xed ≡ ORIG 0x8c064a5）、_289（0xd9 ≡ 0x8c06491）逐字节同。
int SeriaRoomAniDecoEvent::processEvent(CUser& user)    // 0x0827ba52
{
    seriaRoom_AniDeco deco = user.getSeriaRoomAniDecoInfo();
    int itemIdx =
        G_CDataManager()->getSeriaRoomAniDecoRewardItem(deco.m_field2);
    if (itemIdx == 0)
        return 0;

    Inven_Item item;
    CItem* src = G_CDataManager()->find_item(itemIdx);
    if (src != 0)
    {
        // ORIG 0x0827bad5 `mov %eax,-0x5f`：item(@-0x61)+0x02 ⇒ 原码直接写
        // Inven_Item::m_addInfo 字段（非独立 int 局部；make_item 不写 +0x02，
        // CEquipItem/CStackableItem::make_item 写点已核）。
        item.m_addInfo = src->get_index();
        src->make_item(item);
        item.set_add_info(1);

        char title[256];
        strncpy(title,
                g_scriptStringManager_.findString(4, "game_server_msg_291", 0),
                0xff);
        if (item.m_addInfo != 0)
        {
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                g_scriptStringManager_.findString(4, "game_server_msg_289", 0),
                item, 0, (unsigned int)user.getCurCharacNo(),
                // [N1Free91to105 2026-09-11] 负结论：残余唯一差异 = ReqDBSendNewSystemMail
                // 实参中 GetServerGroupToPvP()↔strlen() 的 callee-saved 角色互换
                // （ORIG 0x0827bb58 `mov %eax,%esi` / 0x0827bb68 `mov %eax,%edi` vs
                //  4.4.6 `%edi`/`%esi`；N109 跨版本 callee-saved 分配序）。
                // 已试旋钮：去 `(int)strlen(title)` 显式 cast（P328⑤）⇒ 102=102 不变、
                // esi/edi 未翻转 ⇒ 回滚。真值 NEAR（n1_v.py 口径），ORIG 102 = our 102。
                title, (int)strlen(title), 0,
                static_cast<ENUM_SERVER_GROUP>(user.GetServerGroupToPvP()),
                false, false);
        }
    }
    return 0;
}
