void cago_meterial_item_change(int slot, SIG_LOAD_INVENTORY* sig,
                               Inven_Item* item);

// ORIG 段内联展开（无独立符号）：collectLock 逐段直接内联，用共享
// 函数级临时 lock/ref 槽对齐 ORIG 的 6 个栈槽（frame 0x38 → disp8 编码）。

// ---------------------------------------------------------------------------
// ORIG W 0x084f0145 sz 0x647（报告缺失；objdump 0x084f0145..0x084f078c 逐指令
// 转写）。SIG_LOAD_INVENTORY 各 wire 段（item stride 0x3d，pack(1)）逐件
// _verifyItem；任一失败 → _writeIncorrectItemInventoryForRecovery 并返 0；
// 途中把各段 amplify lock 非零者收集到 lockRefs（stItemLockRef{lock,space,slot}）。
//   段A equip 区  i 0x00..0x0b  item=sig+0x14aa8  TYPE 0 (b1=0,b2=1)
//                 lock@s+0x14ab9 → space 0x12, slot i+0xa；tag "fairpvp_equip"
//   段B           i 0x0a..0x15  item=sig+0xa3a1   TYPE 1 (0,1)
//                 lock@s+0xa3b2 → space 0x03, slot i；tag "equip"
//                 ※ item+0xb 非零 → meterial_item_change(i,sig,item)
//   段C           i 0x00..0x137 item=sig+0x5949   TYPE 1 (1,0)
//                 lock@s+0x595a → space 0x00, slot i；tag "inven"
//   段D           i 0x00..0x97  item=sig+0xa8e4   TYPE 2 (0,0)
//                 lock@s+0xa8f5 → space 0x02, slot i；tag "cargo"
//                 ※ item+0x06 非零 → cago_meterial_item_change(i,sig,item)
//   段E           i 0x00..0xf1  item=sig+0xeed1   TYPE 3 (0,0)
//                 lock@s+0xeee2 → space 0x07, slot i；tag "creature"
//   段F 仅收集     i 0x00..0x09  lock@s+0xa3b2 → space 0x03, slot i
//   段G 仅收集     i 0x00..0x68  lock@s+0x4065 → space 0x01, slot i
// 全部通过返回 1。
// ---------------------------------------------------------------------------
// [b28d-solo-LinkClosureD] DIFF→N 封顶 | 已收：P183 去 item 指针缓存（每点重发
// imul/add 链，gap 372→424 vs ORIG 427）+ P171/P191 预清与双初始化（lock/i/err
// 槽序 -0x11/-0x10/-0xc 全等）。残余 = 4.1 对部分段 `if(x!=0)` 物化
// `setne;test;je`（4.4 直跳，setcc-fold 负例；P225 while+break 实测可造该物化但
// 净增 23 条、助记符差异不减，故回退）+ 少量常量折叠（ORIG 0x14ab0+9 合并 vs
// 我们 0x14aa0+8+11，N4/P30）。语义与 ORIG 逐段一致（ORIG 0x084f0145 sz0x647）。
