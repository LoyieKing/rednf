// ---- DeleteItem（ORIG T 0x85c68b4，报告 DeleteItem.md L15-440 全文）----
// 按 item space 四路删除来源槽：0=背包(INVEN_TYPE 1, count=+0x07)、1=avatar(reason 4，
// 成功补发事件)、2=cargo、3=装备栏(INVEN_TYPE 0，成功 send_equip)；其余直接成功。
// [b28d-24] DIFF 残282: 大函数帧/槽分配 + 分支块序（N82/P112）。ORIG 0x085c68b4
// [b28d-solo-CPrivateStoreMgr] P167 归零。残 357 行 = cMyTrace 具名对象发射位置
//（ORIG C1 在 getCurCharacNo/Name 之后，P227）+ 帧 0x9c vs 0xac（N82）+ N18a
// 单段直落（`mov 0x2(%eax),%edi` vs 我们两段）。
// [K2 2026-09-11 复验] fresh .o：291=291 助记符全等；18 组 slot-pair = +16×10 /
// -80×8 ⇒ 两组**互换**（ORIG 4 标量 -0x78..-0x6c 深区、两 trace 临时 -0x68/-0x58
// 浅区；我方标量 -0x28..-0x1c 浅、trace -0x78/-0x68 深）= N82附「4.4 聚合对象
// 优先占低槽、标量分区排布」；POD 聚合形已实测反劣（+10 条）⇒ 维持封顶。
bool CPrivateStore::DeleteItem(PrivateStoreItem& item)
{
    // [L14] 已测（回退）：4.4 对标量分区排布（标量落浅区、聚合占低槽），把 4 个标量
    // 收进 POD 聚合虽使它们回到 ORIG 的 -0x78..-0x6c，但 4.4 额外发 4 对
    // `mov -0x78,%eax; mov %eax,-0x88` 拷贝 ⇒ 帧 0x9c→0xac、+10 条 ⇒ 回退，判 N82附。
    int sellPrice = item.m_field40;
    int resealCount = (int)item.m_item.GetReSealCount();
    int upgrade = (int)item.m_item.GetUpgrade();
    int addInfo2 = (int)item.m_item.m_addInfo2;
    // [b28d-CPrivateStoreMgr] P0：ORIG 0x085c6978 起 `movzbl 0x46(%eax),%eax;
    // movsbl %al,%eax` 无槽往返 ⇒ itemIdx/space 都是表达式右值（无具名局部）；
    // 分派是 4.1 的 switch 二分决策树（cmp $1/je case1 + cmp $1/jg + test/je
    // case0 + jmp default，右子树 cmp $2/je + cmp $3/je），4.4 对同构 switch
    // 逐条同形（探针 /tmp/probe_sw.cpp 双编译器实证）；case 叶块物理序
    // 0,2,3,1 = ORIG 地址序（P324），故按该序书写。

    // [b28d-solo] P227 全表达式临时链：ORIG 0x085c68f2 段先把 getCurCharacName/
    // getCurCharacNo/… 求值入 edi/esi/ebx，再 call cMyTrace C1；具名对象令 4.4
    // 先构造再求值（C1 在 -0x40，ORIG 在 -0x68）。
    cMyTrace(__PRETTY_FUNCTION__, 0x4d7, 0)(
        "PrivateStore del charac_name(%s)(%d), item_id(%d), add_info(%d), "
        "upgrade(%d), reseal_count(%d), sell_price(%d)",
        m_pSeller->getCurCharacName(), m_pSeller->getCurCharacNo(),
        item.m_item.m_addInfo, addInfo2, upgrade, resealCount, sellPrice);

    switch ((signed char)item.m_field46)
    {
    case 0:
        if (m_pSeller->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)1, (short)item.m_field48, (int)item.m_item.m_addInfo2,
                (eItemDelReason)0xb, true) != 1)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x4e0, 5)(
                "Store::user_sell_item, delete_item failed , User ch=%d , %d %d",
                m_pSeller->getCurCharacNo(), (short)item.m_field48,
                (int)item.m_item.m_addInfo2);
            m_errorHandler.Do(0x11);
            return false;
        }
        break;
    case 2:
        if (m_pSeller->getCurCharacCargoW()->delete_item(
                (short)item.m_field48, (int)item.m_item.m_addInfo2,
                (eItemDelReason)0xb) != 1)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x4ec, 5)(
                "Store::user_sell_item, delete_item failed 2, User ch=%d , %d %d",
                m_pSeller->getCurCharacNo(), (short)item.m_field48,
                (int)item.m_item.m_addInfo2);
            m_errorHandler.Do(0x11);
            return false;
        }
        break;
    case 3:
        if (m_pSeller->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)0, (short)item.m_field48, 1,
                (eItemDelReason)0xb, true) != 1)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x4f9, 5)(
                "Store::user_sell_item, delete_item failed 3, User ch=%d , %d",
                m_pSeller->getCurCharacNo(), (short)item.m_field48);
            m_errorHandler.Do(0x11);
            return false;
        }
        m_pSeller->send_equip((short)item.m_field48);
        break;
    case 1:
        if (m_pSeller->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)2, (short)item.m_field48, 1,
                (eItemDelReason)4, true) != 1)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x515, 5)(
                "Store::user_sell_item, delete_item failed 3, User ch=%d , %d",
                m_pSeller->getCurCharacNo(), (short)item.m_field48);
            m_errorHandler.Do(0x11);
            return false;
        }
        m_pSeller->getCurCharacInvenR()->SendAvatarEvent(
            0x36, (int)item.m_item.m_addInfo2, 3, (short)item.m_field48 + 0xa, 0);
        break;
    }
    return true;
}