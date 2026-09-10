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
        if (sig->m_invenType1[i].m_fieldb != 0 ||
            sig->m_invenType1[i].m_fieldd != 0)
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
        // ORIG *(int*)(item+0x06)：m_attr(+0x06)+m_addInfo2 低 3 字节槽非零判定
        if (sig->m_cargo[i].m_attr != 0 ||
            (sig->m_cargo[i].m_addInfo2 & 0xffffff) != 0)
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
