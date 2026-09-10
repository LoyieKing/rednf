int LineFunc(int code, const char* file, int line, unsigned int flag);

// ============================================================================
// 装备升级卡合成（ORIG 0x08227e00 T，约 710 指令）
//
// 包协议（回包 header 0x169）：
//   材料 = 升级卡(slotCard) + 目标装备(slotEquip)（合成配方），放入槽 slotTarget。
//   流程见 docs/class_func_reports/Dispatcher_Compound_Equipment_Upgrade_Card。
//
// [b28d-solo-Dispatcher_Compound_Equipment_Upgrade_Card_impl] dispatch_sig 判词：
//   ORIG 0x08227e00 T 709 insn（orig_insns>500 ⇒ §6.2 三件套口径）。
//   ① p167 调用点差集 CLEAR（81/81，0 missing / 0 extra）；
//   ② gap=3（ORIG 709 / 我方 706）；③ 栈槽集合与 ORIG 全等（-0xa..-0x198）。
//   ⇒ 语义完成、byte 层 N 封顶。
//   已修语义缺口（p167 差集定位）：set_ReturnSendPacket 内联→成员调用 ×3（MISSING -3，
//     同时消 put_byte/put_short/put_int EXTRA 各 3）；getCurCharacInvenR/W 逐调用点内联
//     （P0，MISSING -4/-2）；cardIdx/equipIdx 局部→直传字段地址；items[2] 数组（倒计数
//     隐式构造 ×2，0x08227ebf）+ rCard/rEquip 复用原槽；insertedSlots 非 const 迭代器
//     （P96，消 EXTRA 4 项）；out 循环缓存 end（P13）+ iterator→const_iterator 转换构造；
//     find_item 实参内联 + rew.m_addInfo/put_item_idx 字段口径（0x08228729/0x082288d7）；
//     slot 对结构体化复现 `lea -0x2c; add $0x2`；删 ORIG 不存在的 slot 预清与 booster 局部。
//   残余 N（语义等价、跨版本不可复现）：
//     N7 间接 call 目标寄存器——4.4.7 恒 %eax，ORIG 4.1 用 %edx（0x082281bf GetItemType、
//        0x08228749 make_item；本 TU 未路由 4.4.6，P230 分诊）。
//     跨版本 lowering——`if (slot >= 0)` 负数判定 ORIG 0x0822883f 为
//        `movzwl; not; shr $0xf,%ax; test %al,%al; je`（5 条），4.4 恒 `cmpw $0; js`（2 条）；
//        探针 /tmp/s303_probe.s、/tmp/s303_probe2.s 七形（<0 / >=0 / (ushort)>=0x8000 /
//        (uint)(ushort)>>15 / ~x>>15 等）全败。
//     末尾 1 条对齐 nop 属 §6.1 豁免。
// ============================================================================
