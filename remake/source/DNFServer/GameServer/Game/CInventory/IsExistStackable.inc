bool CInventory::IsExistStackable(int itemIdx, Inven_Item::ITEM_TYPE type) const
{
    // b28d-37 [已归零，ORIG 0x0850a016]：旧「跨版本块序封顶」判词作废——提前
    // `if (field != itemIdx) continue;` 形（P223② 同层 continue）令 4.4 复现 ORIG
    // 块序 BODY(cmp)/`je HIT`/ADDL/`jmp COND`/HIT(mov$1)/COND，36=36 逐条全等
    // （仅 .o 未解析 call 重定位伪差）。直 return 形产 `jne COND`+HIT 内联。
    int start;
    int end;
    StartEndPos(type, start, end);
    for (int i = start; i <= end; i++)
    {
        // b28d-37：ORIG 0x0850a05e 块序为 BODY(cmp)/`je HIT`/ADDL/`jmp COND`/
        // HIT(mov$1)/COND；4.4 直 return 形产 `jne COND`+HIT 内联（块序反）。
        // P223② 提前 continue 形试复现 HIT 落 ADDL 之后。
        if (m_pEquipSlot[i].m_addInfo != itemIdx)
        {
            continue;
        }
        return true;
    }
    return false;
}