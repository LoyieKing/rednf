// [b28d-11b] 修法：village 声明前置翻槽序（4.4 先声明深/4.1 先声明浅）+ while 复现 sete 物化 → IDENTICAL 73 @0x086cec84
bool GameWorld::isDungeonEntranceArea(CUser* user)
{
    // b28d-lane11：ORIG 0x086cec8a `movl $0x0,-0x10(%ebp)` = Area* area 函数头
    // 声明置空（后覆写，P197③ 两段式）；trace 改 P221 全表达式临时链翻槽序；
    // 尾部 `!=4 return false` 反极性复现 ORIG je 落 mov$1 岛。
    // 残余封顶：①area/village 槽互换（ORIG area=-0x10/village=-0x14 系 4.1
    // 声明序；4.4 首次使用序反，声明序/两段式试形均不可翻，X1 负例）；
    // ②两处 `cmpl $0,槽; sete %al; test; je` 槽比零物化系 4.1 产物，4.4 恒
    // `cmp/jne` 直跳（P197④）⇒ 封 N82/N197。
    Village* village;
    Area* area = NULL;
    village = getUserVillage(user);
    while (village == NULL)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1685, 5)("getUserVillage call failed");
        return false;
    }
    int areaNo = user->get_area(false);
    area = village->getArea(areaNo);
    while (area == NULL)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x168c, 5)("getArea call failed (area no:%d)", areaNo);
        return false;
    }
    if (user->get_charac_level() < 2)
    {
        return true;
    }
    if (area->m_areaType != 4)
    {
        return false;
    }
    return true;
}