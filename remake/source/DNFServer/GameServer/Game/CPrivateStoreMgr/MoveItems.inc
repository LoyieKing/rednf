// ---- MoveItems（ORIG T 0x85c7246，报告 MoveItems.md L15-333 全文）----
// 两轮扫描：state==1 条目经 GetItemInfo 刷新后置 2 并校验（avatar 校验剩余期，
// 其余回写 +0x07）；CheckTotalItemPrice 通过后把 state==2 条目 DeleteItem 置 3。
// [b28d-24] DIFF 残191: P167 MISSING vector::operator[]×2（访问方式差异）。ORIG 0x085c7246
// [b28d-solo-CPrivateStoreMgr] P167 归零（第二循环逐次 op[]×3）；P191 双初始化 +
// P292 档① 正向包裹已应用（两循环）。残 202 行 = N4/P30 寻址（ORIG `mov this,%edx;
// add $0x3c,%edx` vs 4.4 `mov this,%eax; lea 0x3c(%eax),%edx`）+ i 槽差 4
//（N98/N95）+ trace 具名对象槽（P227）。
bool CPrivateStore::MoveItems()
{
    // [b28d-solo-CPrivateStoreMgr] P191 双初始化 + P292 档① 正向包裹：
    // ORIG 0x085c724d/0x085c7254 两条 `movl $0x0,-0x10`（i 声明+for 初始化）；
    // 0x085c727d `cmp $1,%al; sete %al; test; je <循环尾>` ⇒ `==1` 正向包裹。
    // [b28d-solo] ORIG 0x085c727d `cmp $0xd,-0x10; setle` 有符号 + 每处
    // `mov i,%eax; mov this,%edx; add $0x3c,%edx`（值先）⇒ i 须为 int（P2/P218）。
    int i = 0;
    for (i = 0; i < 0xe; ++i)
    {
        if (m_items[i].GetState() == 1)
        {

        int prevAddInfo2 = (int)m_items[i].m_item.m_addInfo2;
        // [b28d-solo] 正极性包裹复现 ORIG 0x85c72d3 `call; test al,al;
        // je 0x85c7399`（4.1 对 !bool 直判不物化；4.4 的 if(!f()) 恒 xor $1
        // 并把失败块落到 fall-through，块序全反）。
        if (GetItemInfo(m_pSeller, m_items[i]))
        {
            m_items[i].m_flag3d = 2;

            // [b28d-solo] ORIG 0x85c730d `cmp al,8; setne al; test al,al;
            // je 0x85c7336` ⇒ 源码是否定式 if (field1 != AVATAR) 回写、else
            // 走 avatar 分支（P133 补充：否定式才产 setne 物化）。
            if ((int)m_items[i].m_item.m_field1 != Inven_Item::ITEM_TYPE_AVATAR)
            {
                m_items[i].m_item.m_addInfo2 = prevAddInfo2;
            }
            else
            {
                // P0：ORIG 0x85c7370 `test eax,eax` 直判（expire 无槽往返）
                // ⇒ 不声明具名 expire 局部，表达式内联。
                if (m_pSeller->getCurCharacInvenR()->GetAvatarItemMgrR()
                        ->GetExpireDate((int)m_items[i].m_item.m_addInfo2) != 0)
                {
                    // 此失败路径不 ResetItems（ORIG 如此）。
                    m_errorHandler.Do(0x11);
                    return false;
                }
            }
        }
        else
        {
            ResetItems();
            m_errorHandler.Do(0x11);
            return false;
        }
        }
    }

    if (CheckTotalItemPrice() != 1)
    {
        ResetItems();
        m_errorHandler.Do(0x11);
        return false;
    }

    // ORIG 0x085c7414 `movl $0x0,-0x10` 单条（复用 i 槽）+ `cmp $2; sete; je` 正向。
    for (i = 0; i < 0xe; ++i)
    {
        // [b28d-solo-CPrivateStoreMgr] P157/P183：ORIG 逐次 operator[]（3 次：
        // GetState / DeleteItem 实参 / 写 +0x3d），旧缓存引用只 1 次 ⇒ P167
        // MISSING operator[]×2。改为每使用点重新下标。
        if (m_items[i].GetState() == 2)
        {
            if (DeleteItem(m_items[i]))
            {
                m_items[i].m_flag3d = 3;
            }
            else
            {
                // [b28d-solo] P221：ORIG trace 槽 -0x20，具名对象排到 -0x24。
                cMyTrace(__PRETTY_FUNCTION__, 0x637, 5)(
                    "CPrivateStore [%s][%d]", __PRETTY_FUNCTION__, 0x637);
                ResetItems();
                m_errorHandler.Do(0x11);
                return false;
            }
        }
    }
    return true;
}