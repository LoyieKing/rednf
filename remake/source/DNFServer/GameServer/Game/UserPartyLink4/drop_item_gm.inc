int CParty::drop_item_gm(CUser* user, unsigned int itemIdx)
{
    if (user->CheckInTrade())
        return 0x13;                                   // ORIG 0x085a73c1 `mov $0x13,%ebx`

    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item == 0)
        return -1;                                     // ORIG 0x085a73e8 `mov $0xffffffff,%ebx`

    // ORIG 0x085a73f2/0x085a7400：map_item ctor（-0xc8）**先于** Inven_Item ctor
    // （-0x71）发射 ⇒ 原码 `map_item drop;` 声明在 `Inven_Item invenItem;` 之前
    // （4.1 ctor 序 = 声明序；我方原把 drop 声明放在调用点处 ⇒ 多出 ctor 重排）。
    map_item drop;
    Inven_Item invenItem;
    invenItem.m_addInfo = (int)itemIdx;
    item->make_item(invenItem);                        // vtable slot make_item
    // ORIG 0x085a742a `mov -0x6f(%ebp),%eax; test %eax,%eax; je 085a754f`：
    // m_addInfo==0 时**跳过** equip 检查（非 early return）。
    // [本波·P225 推广] `&&` 两条件 4.4 恒折直跳（多一条 xor 归一）；拆嵌套 if
    // 复现 ORIG 0x085a742a/0x085a7439 各自独立的 `test/cmp + je`。
    if (invenItem.m_addInfo != 0)
    {
        if (invenItem.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
        {
            // 装备类物品实为 CEquipItem；+0x234 = CEquipItem::m_itemType。
            const CEquipItem* equip = static_cast<const CEquipItem*>(item);
            if (equip->m_itemType == Inven_Item::ITEM_TYPE_INVALID ||
                equip->m_itemType <= Inven_Item::ITEM_TYPE_9)
                return 0x17;                           // ORIG 0x085a745f `mov $0x17,%ebx`
        }
    }

    drop.m_count = 0;                                  // byte +0x00 = 0
    // ORIG 0x085a7470/0x085a747a：m_field1(+0x01)/m_addInfo(+0x02) 的拷贝发射在
    // set_add_info 之前（`movzbl -0x70; mov %al,-0xb7` / `mov -0x6f; mov %eax,-0xb6`）
    // ⇒ 原码这两条赋值写在 set_add_info(1) 之前；后移会整段错位。
    drop.m_item.m_field1 = invenItem.m_field1;         // byte +0x11（ITEM_TYPE）
    drop.m_item.m_addInfo = invenItem.m_addInfo;       // dword +0x12（item idx）
    drop.m_item.set_add_info(1);                       // 内嵌 item.add_info = 1
    drop.m_item.SetItemAttr(invenItem.GetItemAttr());  // +0x16 ← src GetItemAttr
    drop.m_item.m_fieldb = invenItem.m_fieldb;         // word +0x1b ← src endurance
    // ORIG 0x085a74ca：m_field0(+0x00) 的拷贝夹在 m_fieldb 与 m_fieldd 之间
    // （`movzbl -0x71; mov %al,-0xb8` 位于 0x0b/0x0d 两条之间）⇒ 语句序须如此。
    drop.m_item.m_field0 = invenItem.m_field0;         // byte +0x10
    drop.m_item.m_fieldd = invenItem.m_fieldd;         // dword +0x1d ← src union
    // ORIG 0x085a74dd..0x085a7503：m_amp（stAmplifyOption_t，14B）拆为
    // guid 视图三条（+0x15/+0x19/+0x1d）在前、head 视图一条（+0x11）在后
    // ⇒ 原码是两次视图赋值（`m_guid` 先、`m_amp0Head` 后），整体赋值 4.4 恒按
    // 偏移升序（0x11 提前）错位。
    drop.m_item.m_amp.m_guid = invenItem.m_amp.m_guid;         // +0x15/+0x19/+0x1d
    drop.m_item.m_amp.m_amp0Head = invenItem.m_amp.m_amp0Head; // +0x11
    drop.m_item.m_random.m_random =                    // RandomOption 子块整体（14B）
        invenItem.m_random.m_random;
    drop.m_item.m_upgradeSep.SetUpgradeSeparate(
        invenItem.m_upgradeSep.GetUpgradeSeparate());  // +0x43

    int slotIdx = reinterpret_cast<CBattle_Field*>(&m_battleField)
                      ->drop_item(drop);               // 按值传 0x54 字节
    if (slotIdx == -1)
        return 0x16;                                   // ORIG 0x085a758b `mov $0x16,%ebx`

    // [本波·宽度旋钮] ORIG 0x085a7595/0x085a759b 发 `movw $0x12c,-0x20(%ebp)` /
    // `movw $0xf0,-0x1e(%ebp)`（两个 **short 具名局部**），传参时经 short→int
    // 提升 `movswl`（0x085a75e4/0x085a75f7）⇒ 原码是 short 局部而非字面量直传
    // （我方原写 `put_short(300)` 产 `movl $0x12c` 直压）。
    short field12c = 300;
    short fieldf0 = 240;
    PacketGuard packet;
    packet.put_header(0, 0x28);
    packet.put_short((unsigned short)user->get_unique_id());
    packet.put_short(field12c);                        // 0x12c
    packet.put_short(fieldf0);                         // 0xf0
    packet.put_short(slotIdx);
    packet.put_int(drop.m_item.m_addInfo);
    packet.put_byte(drop.m_item.GetItemAttr());
    packet.put_int(drop.m_item.get_add_info());
    packet.put_short(drop.m_item.m_fieldb);
    packet.put_int(drop.m_item.m_fieldd);
    packet.put_byte(drop.m_item.m_amp.getAbilityType());
    packet.put_short(drop.m_item.m_amp.getAbilityValue());
    packet.put_packet(drop.m_item);
    // ORIG 0x085a7703..0x085a7717：`movzbl; test %al,%al; je L; mov $0x0,%eax;
    // jmp E; L: mov $0x1,%eax; E:` —— 4.1 直测无 `xor $1` 归一。写 `x==0 ? 1 : 0`
    // 时 4.4 具 XOR 归一（等价变换为 `x!=0 ? 0 : 1`）⇒ 改按**反极性**直接写
    // `x ? 0 : 1` 复现直测形（语义等值：m_count!=0→0、==0→1）。
    packet.put_byte(drop.m_count ? 0 : 1);
    packet.finalize(true);
    send_to_party(packet);
    return 0;                                          // ORIG 0x085a774b `mov $0x0,%ebx`
