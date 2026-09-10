int GameWorld::check_restrictive_commercial_transaction_zone(int vill, int area,
                                                             int x, int y)
{
    if (check_valid_area(vill, area))
    {
        // [b28d-solo-GameWorld] ORIG 0x086c45d1 `lea -0x14; call pair<int,int>::C1Ev`
        // 在 getArea 之前 ⇒ 具名 pt 先声明（4.1 先声明深：pt=-0x14 深于 pArea=-0xc）。
        std::pair<int, int> pt;
        Area* pArea = m_pVill[vill].getArea(area);
        if (pArea != NULL)
        {
            // [b28d-solo-GameWorld] ORIG 0x086c460f `movzbl 0x6c; xor $0x1; test; je`
            // ⇒ P292 字段 bool + 裸 !field（uchar 恒多 setne/test 或 cmp $1 形）。
            if (!pArea->m_field6c)
            {
                return 0x3e;
            }
            // ORIG 0x86c461e：遍历 Area+0x70 的 vector<pair<int,int>>
            // （m_npcPositions，非 +0x7c 的 m_mapAreas）。
            // [b28d-solo-GameWorld] P283+P13：ORIG 0x086c462e begin(-0x18) →
            // 0x086c4646 end(-0x1c) → 0x086c464e `jmp 条件块` → body → `++it`(ppEv)
            // → 条件块 `!=` 回跳。4.4 对 for 恒条件置顶且每轮重调 end() ⇒ 用
            // 具名 it/e + goto 尾条件复现；`pt = *it` 逐字段拷贝（ORIG 0x086c465b）。
            std::vector<std::pair<int, int> >::iterator it =
                pArea->m_npcPositions.begin();
            std::vector<std::pair<int, int> >::iterator e =
                pArea->m_npcPositions.end();
            goto COND;
        LOOP:
            pt = *it;
            if (Is_restrictive_commercial_transaction_zone(
                    pt.first, pt.second, x, y))
            {
                return 0x52;
            }
            ++it;
        COND:
            if (it != e)
            {
                goto LOOP;
            }
        }
    }
    return 0;
