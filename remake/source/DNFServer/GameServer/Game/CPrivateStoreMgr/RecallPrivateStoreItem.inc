// ---- RecallPrivateStoreItem（ORIG T 0x85c63ca，报告 RecallPrivateStoreItem.md L15-513 全文）----
// 第一轮把 state==3 条目按类型插回 avatar 背包(reason 0xa)/普通背包(reason 0x12)，
// 回填槽号后置 4；任一插入负值中断转第二轮回退 state==4→3；全部成功 ResetItems。
// [b28d-24] DIFF 残173: P167 MISSING getCurCharacName×1（真缺口）。ORIG 0x085c63ca
// [b28d-solo-CPrivateStoreMgr] P167 归零（getCurCharacName 两次独立调用，P48）。
// 残 281 行 = 帧 0x140 vs 0xd0（N82，4.1 不复用槽）+ N4 寻址（add $0x3c vs lea）
// + P292 极性（sete vs setne）。P191 双初始化+P292 正向包裹实测 281→285 劣化
//（已回退）⇒ 封顶。
// [L14] 本轮复核：321/321 助记符序列全等、0 差异块；残 187 行全为槽号
// （0x3d 逐字段拷贝块：我方 -0x6d.. 与 ORIG 相差 0x20 平移）⇒ N82/N95 帧槽族封顶。
// [K2 2026-09-11 复验·修正] 精确残差 = 64 组 slot-pair（-61×32 / +61×32）⇒
// 两个 Inven_Item 栈副本（0x3d=61B：ORIG avatar 组深 -0xaa、normal 组浅 -0x6d）
// **整体互换** = N93/P212「4.4 块内先声明浅 vs 4.1 先声明深」，旧「0x20 平移」说
// 作废；提前声明两对象需默认 ctor（每轮多发 C1Ev，p167 将现 EXTRA）⇒ 维持封顶。
// （指令数 321=321、sub 0x140 双侧相同。）
bool CPrivateStore::RecallPrivateStoreItem()
{
    // [BigStore P191/P2] ORIG 0x085c63d5/0x085c63dc 双 `movl $0x0,-0x20`，尾测
    // `addl $1; cmpl $0xd; setle`（有符号）⇒ i 为 int、声明+for 各初始化一次。
    int i = 0;
    for (i = 0; i < 0xe; ++i)
    {
        PrivateStoreItem& item = m_items[i];
        // [BigStore P292] ORIG 0x085c6423 `cmp $0x3,%al; sete %al; test; je 循环尾`
        // ⇒ `==3` 正向包裹（旧 !=3+continue 反形产 setne/jne）。
        if (item.GetState() == 3)
        {
            int slot = -1;
            if ((int)item.m_item.m_itemType == Inven_Item::ITEM_TYPE_AVATAR)
            {
                // [b28d-solo] ORIG 0x85c642e 先把 item.m_item 逐字段拷到栈副本 -0xaa，
                // 再按值拷入 outgoing 槽（两次拷贝，4.1 对按值实参的 lowering）⇒
                // 显式具名副本复现。
                // [K2 2026-09-11 实测回滚] 分支体加块作用域（P212 块级槽重开）：
                //   fresh .o 残差仍 64 行（-61×32/+61×32 不变）⇒ 4.4 槽向不由块深
                //   决定 ⇒ 回滚。
                Inven_Item itemCopy = item.m_item;
                // [BigStore P329] ORIG 0x085c64bb getCurCharacInvenW 结果直落
                // insertAvatarIntoInventory this 实参槽（16+2 参），无 invenW 缓存槽。
                slot = m_pSeller->getCurCharacInvenW()->insertAvatarIntoInventory(
                    itemCopy, (eAvatarItemAddReason)0xa, false);
                if (slot < 0)
                    break;
                m_pSeller->getCurCharacInvenR()->SendAvatarEvent(
                    0x36, (int)item.m_item.m_addInfo2, 0, slot + 0xa, 0);
            }
            else
            {
                // [b28d-solo] 同上：ORIG 0x85c6xxx 两次逐字段拷贝（-0x6d 副本）。
                Inven_Item itemCopy = item.m_item;
                slot = m_pSeller->getCurCharacInvenW()->insertItemIntoInventory(
                    itemCopy, (eItemAddReason)0x12, false, true);
            }
            if (slot < 0)
                break;

            PrivateStoreItem info;
            info.m_field48 = (unsigned short)slot;   // 先写槽号 +0x48 再从背包回填数据
            GetItemInfo(m_pSeller, info);

            int addNew = 0;
            int addOld = 0;
            if (item.m_item.isEquipableItemType())
            {
                addNew = 1;
                addOld = 1;
            }
            else
            {
                addNew = (int)info.m_item.get_add_info();
                addOld = (int)item.m_item.get_add_info();
            }
            // [b28d-solo-CPrivateStoreMgr] P48/P157：ORIG 对同一 getter 连调两次
            // （P167 差集 getCurCharacName×2，旧缓存形只 1 次）⇒ 实参处各写一次。
            m_pSeller->m_historyLog.PrivateStoreItemAdd(
                (INVEN_TYPE)1, item.m_item, addNew, addOld, (eItemAddReason)0x12,
                m_pSeller->getCurCharacName(), m_pSeller->getCurCharacName());

            item.m_field48 = (unsigned short)slot;
            item.m_flag3d = 4;
        }
    }

    if (i != 0xe)
    {
        // [BigStore P191] 第二循环复用同一 i 槽（ORIG 再发双初始化 -0x20）。
        for (i = 0; i < 0xe; ++i)
        {
            PrivateStoreItem& item = m_items[i];
            // [BigStore P292] ORIG 第二循环同形 `cmp $4; sete; je` ⇒ 正向包裹。
            if (item.GetState() == 4)
            {
                if (DeleteItem(item))
                {
                    // [b28d-solo] P221：ORIG trace 槽 -0x28，具名对象排到 -0x48。
                    cMyTrace(__PRETTY_FUNCTION__, 0x4c9, 5)(
                        "[%s][%d]", __PRETTY_FUNCTION__, 0x4c9);
                }
                item.m_flag3d = 3;   // DeleteItem 成败皆置
            }
        }
        return false;
    }

    ResetItems();
    return true;
}