// [b23S3C] ORIG 0x0860a34a 尾 `leave; ret` 无 mov $0,%eax ⇒ 返回 void；
// getCurCharacInvenR/CargoR 结果 ORIG 直落 0x4(%esp) 实参槽（无 -off 往返）⇒ P48 去具名局部；
// 第一段 ORIG 先 Inven_Item ctor(-0x4d) 再 sret 临时(-0xd8)+逐字段赋值 ⇒ 拆 ctor/赋值两句
// （初始化形被 copy-elision 直构进 item 槽，丢 ctor call + 16 条拷贝 = gap 30 的真因）
void CSkillChanger::_GetUserItemCount(const CUser* user)
{
    for (int i = 3; i < 0x69; i++)
    {
        Inven_Item item;
        item = ((CInventory*)user->getCurCharacInvenR())->GetInvenSlot(1, i);
        _CountSkillBook(item);
    }
    for (int i = 0; i < 0x98; i++)
    {
        Inven_Item item;
        item = ((const CCargo*)user->getCurCharacCargoR())->get_cargo_slot(i);
        _CountSkillBook(item);
    }
}