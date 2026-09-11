// ---------------------------------------------------------------------------
// ORIG W 0x084f0145 sz 0x647（报告缺失；objdump 0x084f0145..0x084f078c 逐指令
// 转写）。SIG_LOAD_INVENTORY 各 wire 段（item stride 0x3d，pack(1)）逐件
// _verifyItem；任一失败 → _writeIncorrectItemInventoryForRecovery 并返 0；
// 途中把各段 amplify lock 非零者收集到 lockRefs（stItemLockRef{lock,space,slot}）。
//   段A equip 区  i 0x00..0x0b  item=sig+0x14aa8  TYPE 0 (b1=0,b2=1)
//                 lock@s+0x14ab9 → space 0x12, slot i+0xa；tag "fairpvp_equip"
//   段B           i 0x0a..0x15  item=sig+0xa3a1   TYPE 1 (0,1)
//                 lock@s+0xa3b2 → space 0x03, slot i；tag "equip"
//   段C           i 0x00..0x137 item=sig+0x5949   TYPE 1 (1,0)
//                 lock@s+0x595a → space 0x00, slot i；tag "inven"
//                 ※ [Q7S3 修正] 守卫读绝对 0x594b（= item+0x02 = m_addInfo）
//                   → meterial_item_change(i,sig,item)；旧注 "item+0xb" 系
//                   把「相对 0x5940 基址的偏移」误当 item 偏移（真 bug 已修）。
//   段D           i 0x00..0x97  item=sig+0xa8e4   TYPE 2 (0,0)
//                 lock@s+0xa8f5 → space 0x02, slot i；tag "cargo"
//                 ※ [Q7S3 修正] 守卫读绝对 0xa8e6（= item+0x02 = m_addInfo）
//                   → cago_meterial_item_change(i,sig,item)；旧注 "item+0x06"
//                   同上为基址偏移误记（真 bug 已修）。
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
bool _verifyInventory(SIG_LOAD_INVENTORY* sig,
                      std::vector<item_lock::stItemLockRef>& lockRefs,
                      CUser* user)
{
    // ORIG 0x084f0185..19a 预清（P171 照抄）：lock(-0x11)=0、i(-0x10)=0、err(-0xc)=0、
    // 随后 `movl $0x0,-0x10` 二次初始化（P191 双初始化形）。
    // ORIG 每处访问重发完整 imul 链（P183 地址重物化铁律）⇒ 无 item 指针缓存。
    // ref 赋值序 ORIG 为 m_lock→m_slot→m_space（0x084f019f..1b0）。
    // ORIG 预清序 lock(-0x11) → i(-0x10) → err(-0xc) ⇒ 声明序 lock, i, err。
    unsigned char lock = 0;
    int i = 0;
    int err = 0;
    item_lock::stItemLockRef ref;

    // 段 A（SIG_LOAD_INVENTORY m_expandEquip.m_items，+0x14aa8 起 0x24 项）
    for (i = 0; i <= 0xb; ++i)
    {
        lock = sig->m_expandEquip.m_items[i].m_amp.GetLock();
        if (lock != 0)
        {
            ref.m_lock = lock;
            ref.m_slot = static_cast<short>(i + 0xa);
            ref.m_space = 0x12;
            lockRefs.push_back(ref);
        }
        err = _verifyItem(i, &sig->m_expandEquip.m_items[i], INVENTORY_TYPE_0,
                          true, true, user);
        // [D01 · C05④ 步骤 2 实测负例 2026-09-10] P225 原形（5 处 `if` 整块换
        // `while (err != 0) { _write…; return false; }`）**劣化**：our_insns
        // 424→**434**、gap −3→**+7**（ORIG 427）、TU 四数不变 ⇒ 已回滚。
        // 与 C01 三正例的差别：本处 body 仅「单条调用 + return」、无多分支早退 +
        // 末尾 break 结构 ⇒ while 恒多回边/结构 jmp。⇒ 本件归 **N-88/P197④**
        // （槽值 vs 常量比较物化：ORIG `mov %eax,-0xc; cmpl $0,-0xc; setne; test; je`）。
        if (err != 0)
        {
            _writeIncorrectItemInventoryForRecovery(
                sig->m_characNo, sig->m_expandEquip.m_items[i],
                "fairpvp_equip", err, i);
            return false;
        }
    }
    // 段 B（m_equipMirror）——ORIG 无 meterial_item_change 调用（仅段 C/D 有）
    for (i = 0xa; i <= 0x15; ++i)
    {
        lock = sig->m_equipMirror[i].m_amp.GetLock();
        // ORIG 段 B 为 bool 物化形（cmpb $0; setne %al; test %al,%al; je）
        // 与段 A 的 cmpb $0; je 不同 ⇒ P225 while+break 复现。
        if (lock != 0)
        {
            ref.m_lock = lock;
            ref.m_slot = static_cast<short>(i);
            ref.m_space = 0x03;
            lockRefs.push_back(ref);
        }
        err = _verifyItem(i, &sig->m_equipMirror[i], INVENTORY_TYPE_0,
                          true, true, user);
        if (err != 0)
        {
            _writeIncorrectItemInventoryForRecovery(
                sig->m_characNo, sig->m_equipMirror[i], "equip", err, i);
            return false;
        }
    }
    // 段 C（m_invenType1）
    for (i = 0; i <= 0x137; ++i)
    {
        lock = sig->m_invenType1[i].m_amp.GetLock();
        if (lock != 0)
        {
            ref.m_lock = lock;
            ref.m_slot = static_cast<short>(i);
            ref.m_space = 0x00;
            lockRefs.push_back(ref);
        }
        // [Q7S3 语义轴 · 真 bug 修正] ORIG 0x084f03a1..0x084f03e0：基址取
        // sig+i*0x3d+0x5940（= &m_invenType1[i] - 9，数组 +0x5949 由 m_amp
        // GetLock 传参 0x595a-0x11 双证）后 `mov 0xb(%eax); test %eax,%eax`
        // ⇒ 绝对 0x594b = &m_invenType1[i] + 2 = m_addInfo（4 字节整体判）。
        // 旧写 "item+0xb/item+0xd" 系把「相对 0x5940 基址的偏移」误当 item
        // 偏移（对应绝对 0x5954/0x5956，字段为 m_fieldb/m_fieldd）⇒ 守卫条件
        // 与原版不同（真语义 bug，已修）。
        if (sig->m_invenType1[i].m_addInfo != 0)
            meterial_item_change(i, sig, &sig->m_invenType1[i]);
        err = _verifyItem(i, &sig->m_invenType1[i], INVENTORY_TYPE_1,
                          false, false, user);
        if (err != 0)
        {
            _writeIncorrectItemInventoryForRecovery(
                sig->m_characNo, sig->m_invenType1[i], "inven", err, i);
            return false;
        }
    }
    // 段 D（m_cargo）
    for (i = 0; i <= 0x97; ++i)
    {
        lock = sig->m_cargo[i].m_amp.GetLock();
        if (lock != 0)
        {
            ref.m_lock = lock;
            ref.m_slot = static_cast<short>(i);
            ref.m_space = 0x02;
            lockRefs.push_back(ref);
        }
        // [Q7S3 语义轴 · 真 bug 修正] ORIG 0x084f051c..0x084f0560：基址取
        // sig+i*0x3d+0xa8e0（= &m_cargo[i] - 4，数组 +0xa8e4 由 _verifyItem/
        // cago_ 传参的 `add $0x4` 双证）后 `mov 0x6(%eax); test %eax,%eax`
        // ⇒ 绝对 0xa8e6 = &m_cargo[i] + 2 = m_addInfo（4 字节整体判）。
        // 旧写 "item+0x06/低 3 字节槽" 对应绝对 0xa8ea/0xa8eb（m_attr/
        // m_addInfo2）⇒ 守卫条件与原版不同（真语义 bug，已修）。
        if (sig->m_cargo[i].m_addInfo != 0)
            cago_meterial_item_change(i, sig, &sig->m_cargo[i]);
        err = _verifyItem(i, &sig->m_cargo[i], INVENTORY_TYPE_2, false, false,
                          user);
        if (err != 0)
        {
            _writeIncorrectItemInventoryForRecovery(
                sig->m_characNo, sig->m_cargo[i], "cargo", err, i);
            return false;
        }
    }
    // 段 E（m_invenType3）
    for (i = 0; i <= 0xf1; ++i)
    {
        lock = sig->m_invenType3[i].m_amp.GetLock();
        if (lock != 0)
        {
            ref.m_lock = lock;
            ref.m_slot = static_cast<short>(i);
            ref.m_space = 0x07;
            lockRefs.push_back(ref);
        }
        err = _verifyItem(i, &sig->m_invenType3[i], INVENTORY_TYPE_3,
                          false, false, user);
        if (err != 0)
        {
            _writeIncorrectItemInventoryForRecovery(
                sig->m_characNo, sig->m_invenType3[i], "creature", err, i);
            return false;
        }
    }
    // 段 F（仅收集；m_equipMirror[i].m_amp，+0xa3b2 = 0xa3a1+0x11）
    for (i = 0; i <= 9; ++i)
    {
        lock = sig->m_equipMirror[i].m_amp.GetLock();
        if (lock != 0)
        {
            ref.m_lock = lock;
            ref.m_slot = static_cast<short>(i);
            ref.m_space = 0x03;
            lockRefs.push_back(ref);
        }
    }
    // 段 G（仅收集；ORIG +0x4065 = m_invenType2[i].m_random.m_field2 区，
    // 按 stAmplifyOption_t 视图读取——ORIG 语义保留类型视图）
    for (i = 0; i <= 0x68; ++i)
    {
        lock = reinterpret_cast<const stAmplifyOption_t*>(
            &sig->m_invenType2[i].m_random.m_field2)->GetLock();
        if (lock != 0)
        {
            ref.m_lock = lock;
            ref.m_slot = static_cast<short>(i);
            ref.m_space = 0x01;
            lockRefs.push_back(ref);
        }
    }
    return true;
}