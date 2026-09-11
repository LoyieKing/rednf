// ---- CheckInventoryEquipmentEmptyCount —— T 0x08271062（同名报告全文转写）--
// ORIG 0x08271062：call; leave; ret 穿透。内外配对律（batch-13）：callee
// check_empty_count 终裁 bool ⇒ 本外层同步 bool（probe3 实证 bool+bool 直穿无
// movzbl；int 外 + bool 内必产 movzbl 归一段）。
bool CheckInventoryEquipmentEmptyCount(CUser& user, int need)
{
    return user.getCurCharacInvenR()->check_empty_count(
        (Inven_Item::ITEM_TYPE)1, need);
}