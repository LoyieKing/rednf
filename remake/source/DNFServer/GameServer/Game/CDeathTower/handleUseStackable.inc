bool CDeathTower::handleUseStackable(CUser* user, ENUM_ITEMSPACE itemSpace, unsigned short slot)
{
    // [b28d-solo-CDeathTower] verdict=AE（68/73→归零，P167 CLEAR）：修法 =
    // ①P52/P183 链式 `user->getCurCharacInvenR()->GetInvenRef(...)`（去具名 inven
    // 缓存槽）；②删多余的 `item->m_addInfo != 0x18` 第三短路项（ORIG 0x08466331
    // 只有 `limit && (a<0x1964 || a>6999)`）；③switch 直用 `item->m_addInfo`
    // （P183 每决策点重读）；④正向 if 包裹复现 ORIG 纯直跳链（禁早返/合并 &&）。
    // [b28d-solo-CDeathTower] P52/P183：ORIG 0x08466304 为链式单表达式
    // `user->getCurCharacInvenR()->GetInvenRef(GetInvenTypeFromItemSpace(itemSpace), slot)`
    //（receiver getCurCharacInvenR 在 GetInvenTypeFromItemSpace 之后求值）；
    // 具名 inven 缓存提前 call 入槽 -0x14 且帧 0x30→0x34（P183 禁指针缓存）。
    const Inven_Item* item = user->getCurCharacInvenR()->GetInvenRef(
        GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)itemSpace), slot);
    if (!item)
    {
        return 0;
    }

    // [b28d-solo-CDeathTower] ORIG 0x08466331..0x08466349 短路为
    // `limit && (addInfo < 0x1964 || addInfo > 6999)`（无 `addInfo != 0x18`
    // 第三项；case 0x18 由 switch 决策树自身处理并落 RET1 岛）。
    // 早返两守卫形复现 ORIG 的 `test %al; je RET1` + `jbe SWITCH` +
    // `jbe RET1` 纯直跳链（合并 &&/|| 会被 4.4 物化成 mov$1/$0 岛）。
    if (m_dungeonMgr.getDungeon()->limitOfStackableItemInTower())
    {
        if (item->m_addInfo < 0x1964 || item->m_addInfo > 6999)
        {
            // [b28d-solo-CDeathTower] ORIG 0x08466351 起为 4.1 二分决策树
            // （cmp$0x18 je / ja / cmp$0xa je / …）⇒ 源形是 switch（P46/P163），
            // case 按数值升序书写；case 0x18 由决策树自身处理并落 RET1 岛。
            switch ((unsigned int)item->m_addInfo)
            {
            case 0xa:
            case 0x18:
            case 0x33:
            case 0x28978a:
                break;
            default:
                return 0;
            }
        }
    }
    return 1;
}