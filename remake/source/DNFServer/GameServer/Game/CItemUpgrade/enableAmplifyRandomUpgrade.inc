// ============================================================================
// enableAmplifyRandomUpgrade @ 0x854af38
//   校验随机增幅券并计算随机强化值写入 int& outCount。
//   返回错误码：0=通过, 4=找不到券/物品或非增幅券, 0xd=upgrade 越界,
//   0x13=物品不可增幅。
// [b28d-solo-CItemUpgrade 判词] DIFF（N 封顶，p167 CLEAR）。残余：
//   `_GetUpgradeInfo(...) != 1` ORIG `xor $0x1,%eax; test %al,%al; je`（3 条）
//   vs 4.4 `cmp $0x1,%eax; setne %al; test %al,%al; je`（4 条）——P292/P168a
//   定论：int callee 的调用点写法恒产 setcc，改返回 bool 则 _GetUpgradeInfo
//   尾部（ORIG 0x08546816 `call get_upgrade_info; add; pop; ret` 直返）多归一化
//   ⇒ N 封顶；另帧/槽号差（4.4 对 characLevel 等具名局部槽化）为 N82 族。
// ============================================================================
int CItemUpgrade::enableAmplifyRandomUpgrade(CUser* user, Inven_Item& item,
                                             Inven_Item& ticket,
                                             int& outCount)
{
    // [b28d-solo-CItemUpgrade] ORIG 0x0854af42 无 itemIdx/dm 具名局部（P0）：
    //   mov item,%eax; mov 0x2(%eax),%eax; mov %eax,%ebx（跨 call 保活）;
    //   call G_CDataManager; mov %ebx,0x4(%esp); mov %eax,(%esp); call find_item。
    CItem* itemScript = G_CDataManager()->find_item(item.m_addInfo);
    if (itemScript == 0)
        return 4;

    const upgrade_table_t* table =
        GlobalData::s_itemAmplifier_->getUpgradeInfoTable();
    upgrade_info_t info;              // ORIG +0x04f 直调 upgrade_info_t ctor（禁另加 memset）
    // characLevel 无具名局部：ORIG 在压完 0x10/0xc/0x8/0x4 后直接
    // `call get_charac_level; mov %eax,0x14(%esp)`（P0 内联实参）。
    if (_GetUpgradeInfo(&item, itemScript, *table, &info,
                        user->get_charac_level()) != 1)
        return 0xd;
    // ORIG +0x0a4 死存 GetUpgrade(item)：4.1 保留 call + movzbl + 落槽 -0x1c
    // （P171 死写入照抄）；写 (void)item.GetUpgrade() 会被 4.4 DCE 掉 store。
    int deadUpgrade = item.GetUpgrade();
    (void)deadUpgrade;
    // ORIG +0x0af..+0x0c3 把券信息回填进 info 后三字段（顺序 = 高偏移→低偏移）：
    //   info+0x34=0 / info+0x30=1 / info+0x2c=ticket.m_addInfo。
    info.m_discountBase = 0;
    info.m_meterialCount = 1;
    info.m_recipeItem = ticket.m_addInfo;
    if (itemScript->IsImpossibleAmplify())
        return 0x13;

    // P198① 预清两句式（ORIG +0x0e6 movl $0x0,-0x18 在压参之前）。
    int ret = 0;
    ret = _CheckCondition(user, item, itemScript, ticket, *table, info);
    // [b28d-solo-CItemUpgrade] P225 形：ORIG 0x0854b060 `cmpl $0x0,-0x18;
    // setne %al; test %al,%al; je` 四段物化 ⇒ 源码是 while+break 形
    // （if 形 4.4 恒直跳 je，少 setne/test 两条）。
    while (ret != 0)
    {
        return ret;
    }

    // ORIG +0x143 第二次 G_CDataManager()（不复用函数头），ticketIdx 无具名局部。
    CItem* ticketScript = G_CDataManager()->find_item(ticket.m_addInfo);
    if (ticketScript == 0)
        return 4;
    if (!ticketScript->is_stackable())
        return 4;
    CStackableItem* stackable = static_cast<CStackableItem*>(ticketScript);
    if (!stackable->is_random_amplify_reinforce_ticket())
        return 4;

    unsigned long range = 100;
    int roll = m_rand->randInt(range);
    outCount = stackable->get_amplify_reinforce_count(roll);
    if (outCount < 0 || outCount > 0x1f)
        return 4;
    return 0;
}