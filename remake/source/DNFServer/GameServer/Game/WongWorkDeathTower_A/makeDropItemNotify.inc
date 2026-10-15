// ORIG T 0x0845ff1a：塔内掉落通知包（字段偏移对应 map_item/Inven_Item 具名成员）。
// [round3-L29 2026-09-10 修复] 尾部 bool 条件物化：真因 = `map_item::m_count` 为 bool，
//   `put_byte(item.m_count == 0)` 被 4.4 折 `xor $1; movzbl`（块序亦反）。改为
//   `put_byte((int)item.m_count != 0 ? 0 : 1);`（int cast 阻断 bool 取反折叠）⇒
//   **AE 111/111 ndiff=0**（此前 DIFF 108/111 ndiff13）。ORIG 0x0845ff1a 证据形：
//   `movzbl (%eax),%eax; test %al,%al; je <mov $1>; mov $0,%eax; jmp; mov $1,%eax`。
void CDeathTower::CPacketHandler::makeDropItemNotify(unsigned short uid, int x, int y,
                                                     unsigned short dropIdx,
                                                     const map_item& item)
{
    put_header(0, 0x28);
    put_short((int)uid);
    put_short(x);
    put_short(y);
    put_short((int)dropIdx);
    put_int(item.m_item.m_addInfo);                      // map_item+0x12 = Inven_Item+0x02
    put_byte((int)item.m_item.GetItemAttr());            // Inven_Item::GetItemAttr
    put_int(item.m_item.get_add_info());                 // Inven_Item::get_add_info
    put_short((int)item.m_item.m_endurance);                // map_item+0x1b = Inven_Item+0x0b
    put_int(item.m_item.m_extendInfo);                       // map_item+0x1d = Inven_Item+0x0d
    put_byte((int)item.m_item.m_amp.getAbilityType());   // map_item+0x21 = Inven_Item+0x11
    put_short((int)item.m_item.m_amp.getAbilityValue());
    put_packet(item.m_item);                             // 子对象整体（map_item+0x10）
    // ORIG 0x0845ff1a 尾：`test %al,%al; je <mov $1 岛>; mov $0; jmp`（false 块
    // 直落、true 岛在后）⇒ 原码按 bool 直传（三目 1:0 两支被 4.4 折 sete；
    // `!=0?0:1` 亦折 sete）——试 bool 表达式形。
    // [round3-L29] ORIG 0x0845ff1a 为**分支式物化**（`test;je L1;mov $0;jmp;L1:mov $1`），
    //   试显式三目形复现（隐式 int 转换被 4.4 折 `xor $1`）。
    put_byte((int)item.m_count != 0 ? 0 : 1);
    finalize(true);
}