// ============================================================================
// DB_LoadInventory::GetUserCreature @0x0840ca74
// creature_items 12 列；行数入 +0x1287c（>=0x8e 拒绝）。slot==0xee 映射到
// 0x8c 槽；槽位占用标记 used[0x8d]；重复槽 → Inven_Item 进 overflow vector，
// 遍历后填首个空槽并调 DB_SwapCreatureItem::makeRequest(ui_id, slot)。
// 主槽位（m_invenType3，步长 0x3d）：m_field1=5、m_fieldb=endurance、
// m_addInfo(+0x02) 单条 movl 4B ← it_id（Ghidra 的 8B ulong 表达为伪影，
// GetUserCreature.md 实证 +0x02/+0x07 两条独立 movl）、m_addInfo2=ui_id、
// m_amp lock。详情条目
// （+0x12880，步长 0x38）按 SIG_CREATURE_ITEM_DETAIL_INFO 写点填充。
// 错误行号 0x27f0..0x28e9。
// b28d-solo-DB_LoadInventory：exec/fetch/get_int(0) 共用 bool ok（ORIG -0x1d）
// + 13 处 trace 全表达式临时链（ebx 保活）已归位；p167 差集 MISSING=0。
// [L3CUserPremDBInven 本波落地（ORIG nm 0x0840ca74）] 逐条回 ORIG 同下标后
//   消掉五类差异（差异行 444→112、opcode 匹配 678/731 = 92.7%、insns 735→737）：
//   ① ok 槽：ORIG 有 6 处 `mov %al,-0x1d; movzbl -0x1d`（exec 0x0840cacf /
//      fetch 0x0840cbfe / get_int(0) 0x0840cc9c / get_ulong(1) 0x0840cdb1 /
//      get_int(3) 0x0840ceae / get_int(4) 0x0840cf2c），我方原只在 3 处 ⇒
//      get_ulong(1)/get_int(3)/get_int(4) 恢复 `ok = ...; if (!ok)`（+6 条）。
//   ② slot-overflow 形：ORIG 0x0840cd0x `cmpl $0x8c; jle <+0x29f>` + trace
//      直落 + `jmp <+0xa9d>` ⇒ then=trace/else=主流程书写序（翻转后该块消除）。
//   ③ P250 栈缓冲：ORIG 0840cc6e..0840cc77 单次 `memset(local_220,0,0x74)`；
//      按 ORIG 取址实证建 RowLocals（itId +0x08 / endurance +0x14 / slot +0x2c /
//      uiId +0x30 / name +0x34 / stomach +0x44 / m_field48 +0x48 / exp +0x4c /
//      expire +0x54 / m_field58,+0x5c,+0x60）。D 侧 +0x1c/+0x2c/+0x30/+0x34 由
//      ORIG 栈读（0840d4d2..0840d50e）⇒ 我方改为读 m_field48/58/5c/60（值 0）。
//   ④ detailWord：ORIG 逐字节写（0840d07b byte0 / 0840d172 byte2 / 0840d1ec
//      byte3），byte1 由缓冲 memset 清 0 ⇒ `unsigned char detailWord[4]` 独立
//      槽 + 常量下标（常量下标才会折成 `mov %al,-0xNNN`；struct 成员会退化成
//      `lea base; add $0x50; lea 0x2` 三步链）。
//   ⑤ dupSlot：ORIG 0x0840d339 `cmpb $0x0,-0x11; je` ⇒ char 型 + `== 0`
//      （bool + `!` 在 4.4 产 movzbl/xor 物化，N122 族）。
//   ⑥ `if (dupSlot != 0) 栈副本 else 主槽` 翻转形实测劣化（opcode 匹配
//      681→677、差异行 111→120）⇒ 已回滚，维持 `== 0` 主槽形。
//   ⑦ 重复槽分支的栈副本：ORIG 0x0840d343 `lea -0x1a5(%ebp),%eax;
//      call Inven_Item::C1Ev`（W 0x080cb854）之后**紧跟独立 memset**：
//      0x0840d351 `movl $0x3d,0x8(%esp)` + 0x0840d359 `movl $0x0,0x4(%esp)` +
//      0x0840d361 `lea -0x1a5(%ebp),%eax` + 0x0840d367 `mov %eax,(%esp)` +
//      0x0840d36a `call memset@plt` ⇒ 原码确为
//      `Inven_Item item; memset(&item, 0, 0x3d);`（0x3d=sizeof(Inven_Item)）。
//      [Round7 FillC] 旧判词「ctor 内 reset 已清全字段 ⇒ 无独立 memset」为误判，
//      删除动作已回滚；补后 p167 该 `memset` MISSING(−1) 清零，余 C1/C2、D1/D2
//      同址别名（P93 豁免）。
//   残差（N82/N122 封顶）：我方 742 vs ORIG 731（+11）= extra mov ×13 / add ×6 /
//   movb ×4 / jne ×1 与 missing imul ×9（ORIG 用 `imul $0x3d` 算 m_invenType3
//   步长，4.4 折成 `add $0xeed0` 常量）/ lea ×5 / movl ×2 / je ×1；帧 0x280 vs
//   0x2c0、db -0x34 vs -0x1c。
// ============================================================================
// [L13-group6 判据⑫ 复核] widthscan：clr_l orig=3 / our=12（我方多 9 条
//   `movl $0x0,-N`）+ clr_w 0/1 + cmp_b 1/0 + movzbl 13/14。逐条回 ORIG 同下标
//   核对：ORIG 用**单次 `memset(local_220,0,0x74)`（116B 栈缓冲）合并清零**，
//   我方把同一缓冲拆成 12 个独立具名局部（name[16]/itId/endurance/slot/uiId/
//   stomach/exp/creatureType/noCharge/stat/expire/lockKey）各自 `= 0`。
//   ⇒ 属**结构建模差异**（P250 packed 栈缓冲），非类型宽度问题；**本波已按 ORIG
//   0x74 布局落地**（见上方 ③：RowLocals + 单次 memset，我方 12 条 `movl $0x0`
//   清零消失，差异行 444→111）。
// [IG1 2026-09-10 语义轴落地·used 判定形] ORIG 0x0840cd16/0x0840d556 为
//   `movzbl used[i]; xor $1,%eax; test %al,%al; je` ⇒ 原码是 `if (!used[i])`
//   且元素按布尔语义（P248 档②）；我方原写 `used[i]==1`（多出 `cmp $0x1`，
//   清单 A 组候选）。已改 `char used[]`→`bool used[]` + 两处 `!used[...]`，
//   两处逐条同 ORIG。our_insns 733→735（补 2 条 ORIG 已有的 xor）、
//   ndiff 644→636（严格改善）、gap -2→-4、TU 四数不变 23/62/0/4。
//   余量：dupSlot 判定 ORIG `cmpb $0x0,-0x11` vs 4.4 `xor $1;test;je`（N122 族）。
bool DB_LoadInventory::GetUserCreature(SIG_LOAD_INVENTORY* sig)
{
    static const int handleIdx = 3;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect slot,it_id,name,stomach,exp,endurance,creature_type,"
                  "ui_id,no_charge,stat,item_lock_key,"
                  "unix_timestamp(expire_date) from creature_items"
                  " where charac_no=%u and stat = 0 and expire_date>now()",
                  sig->m_characNo);
    // ORIG：exec/fetch/get_int(0) 共用函数级 bool 槽 -0x1d（`mov %al,-0x1d;
    // movzbl -0x1d`），其余 get_* 为内联 xor 判（无槽）⇒ 源码用同一 ok 变量。
    bool ok = db->exec(true);
    if (!ok)
    {
        // P227 全表达式临时链：实参在 ctor 前求值入 %ebx 跨 ctor 保活。
        cMyTrace(__PRETTY_FUNCTION__, 0x27f0, 5)
            ("DB_LoadInventory::GetUserCreature, exec() ERROR charac_no=%u",
             sig->m_characNo);
        return 0;
    }
    sig->m_creatureCount = db->get_n_rows();
    int row = 0;
    if (sig->m_creatureCount >= 0x8e)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x27fb, 5)
            ("DB_LoadInventory::GetUserCreature, creature_item_count overflow"
             " ERROR charac_no=%u, item_count=%d", sig->m_characNo,
             sig->m_creatureCount);
        return 0;
    }

    // ORIG 两处判定均为 `movzbl used[i]; xor $1; test %al; je`（0x0840cd16 /
    // 0x0840d556）⇒ 元素按布尔语义（P248 档②：字段 bool 化 + 裸 `if(!used[i])`）；
    // bool 数组 sizeof/寻址同 char（1B），memset/写 1 形不变。
    bool used[0x8d];
    memset(used, 0, sizeof(used));
    std::vector<Inven_Item> overflow;
    // ORIG 0x0840d339 为 `cmpb $0x0,-0x11(%ebp); je`（内存直接比较）⇒ 原码
    // 是 char 语义 + `== 0` 判（bool + `!` 在 4.4 产 movzbl/xor 物化，N122 族）。
    char dupSlot = 0;
    for (row = 0; row < sig->m_creatureCount; ++row)
    {
        ok = db->fetch();
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x280e, 5)
                ("DB_LoadInventory::GetUserCreature, fetch() ERROR"
                 " charac_no=%u,%d", sig->m_characNo, row);
            break;
        }
        // ORIG 0840cc6e..0840cc77 为**单次** `memset(local_220,0,0x74)` 合并清零
        // 下列取址传引用的行内变量（0x74 = 116B）；各字段偏移由 ORIG 取址实证：
        // itId +0x08 / endurance +0x14 / slot +0x2c / uiId +0x30 / name +0x34 /
        // stomach +0x44 / exp +0x4c / expire +0x54（缓冲 -0x21c..-0x1a8）。
        // P250：旧译拆成 12 个独立具名局部各自 `= 0`（12 条 movl $0x0），与 ORIG
        // 单 memset 不等价（L13-group6 宽扫列首位）；按 ORIG 布局重建缓冲。
        struct RowLocals
        {
            char m_pad00[0x08];
            unsigned long itId;         // +0x08（get_ulong(1)）
            char m_pad0c[0x08];
            unsigned short endurance;   // +0x14（get_ushort(5)）
            char m_pad16[0x16];
            int slot;                   // +0x2c（get_int(0)）
            int uiId;                   // +0x30（get_int(7)）
            char name[0x10];            // +0x34（get_str(2,,0xd)，含 +0x41..0x43 pad）
            int stomach;                // +0x44（get_int(3)）
            int m_field48;              // +0x48（ORIG 仅被 memset 清 0，D+0x1c 源）
            int exp;                    // +0x4c（get_int(4)）
            char m_pad50[0x04];         // +0x50（detailWord 独立同址槽，见下）
            int expire;                 // +0x54（get_int(11)）
            int m_field58;              // +0x58（memset 清 0，D+0x2c 源）
            int m_field5c;              // +0x5c（memset 清 0，D+0x30 源）
            int m_field60;              // +0x60（memset 清 0，D+0x34 源）
            char m_pad64[0x10];
        } rowBuf;
        // ORIG 无 movl $0x0 清零：这些变量的值总由 get_* 写出（失败即 return）。
        int creatureType, noCharge, stat;
        memset(&rowBuf, 0, sizeof(rowBuf));
        // ORIG 的 detailWord 落在缓冲 +0x50（-0x1cc），由该次 memset 清 0；4.4 下
        // 若把它做进 struct，字节写会退化成 `lea base; add $0x50; lea 0x2` 地址链；
        // 独立 byte[4] 槽 + 常量下标则折成 ORIG 0840d07b/d172/d1ec 的
        // `mov -src,%eax; mov %al,-0x1cc(%ebp)` 形（byte1 保持 0）。
        unsigned char detailWord[4] = {0, 0, 0, 0};

        ok = db->get_int(0, rowBuf.slot);
        // dup 检测在 get_int 失败检查之前（ORIG 0x282c 出口顺序）。
        // ORIG 0840cc9f 段：`mov slot,%eax; mov %eax,-0xc; cmpl $0xee; jne;
        // movl $0x8c,-0xc; cmpl $0x8c,-0xc; jle` ⇒ 原码是「先赋值再条件
        // 覆盖」的 if 形（非三目），且判定为 `<= 0x8c`（jle 正向）。
        int checkSlot = rowBuf.slot;
        if (checkSlot == 0xee)
        {
            checkSlot = 0x8c;
        }
        if (checkSlot > 0x8c)
        {
            // 实验：then=trace / else=主流程（诱 4.4 布 trace 直落，见 ORIG
            // 0x0840cd0x `jle <+0x29f>` + trace + `jmp <+0xa9d>`）。
            cMyTrace("bool DB_LoadInventory::GetUserCreature("
                     "SIG_LOAD_INVENTORY*)", 0x281d, 5)
                ("DB_LoadInventory::GetUserCreature, charac_no=%u, slot=%d"
                 " overflow", sig->m_characNo, checkSlot);
        }
        else
        {
            // ORIG 0x0840cd16/0x0840cd1e/0x0840cd23：`movzbl used[slot]; xor $1;
            // test %al; je <dupSlot 块>` ⇒ 原码是 `if (!used[checkSlot])` 形
            // （X 族 bool 取反物化，P168a/P22），非 `== 1`；then=used 置 1 直落、
            // else=dupSlot=true 在 je 目标处（used 值域 {0,1}，两写法语义等价）。
            if (!used[checkSlot])
            {
                used[checkSlot] = 1;
            }
            else
            {
                dupSlot = 1;
            }
            if (!ok)
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x282c, 5)
                    ("DB_LoadInventory::GetUserCreature, get_int(0) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            ok = db->get_ulong(1, rowBuf.itId);
            if (!ok)
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x2833, 5)
                    ("DB_LoadInventory::GetUserCreature, get_ushort(1) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (!db->get_str(2, rowBuf.name, 0xd))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x2846, 5)
                    ("DB_LoadInventory::GetUserCreature, get_int(2) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            ok = db->get_int(3, rowBuf.stomach);
            if (!ok)
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x284e, 5)
                    ("DB_LoadInventory::GetUserCreature, get_int(3) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            ok = db->get_int(4, rowBuf.exp);
            if (!ok)
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x2855, 5)
                    ("DB_LoadInventory::GetUserCreature, get_int(4) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (!db->get_ushort(5, rowBuf.endurance))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x285b, 5)
                    ("DB_LoadInventory::GetUserCreature, get_ushort(5) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (!db->get_int(6, creatureType))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x2862, 5)
                    ("DB_LoadInventory::GetUserCreature, get_int(6) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            // ORIG 0x0840d07b：`mov -0x158(%ebp),%eax; mov %al,-0x1cc(%ebp)`
            // （+0x50 = detailWord byte0 = creature_type，紧跟 get_int(6) 检查后）。
            detailWord[0] = static_cast<unsigned char>(creatureType);
            if (!db->get_int(7, rowBuf.uiId))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x2869, 5)
                    ("DB_LoadInventory::GetUserCreature, get_int(7) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (!db->get_int(8, noCharge))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x2870, 5)
                    ("DB_LoadInventory::GetUserCreature, get_int(8) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            // ORIG 0x0840d172：byte2 = no_charge。
            detailWord[2] = static_cast<unsigned char>(noCharge);
            if (!db->get_int(9, stat))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x2878, 5)
                    ("DB_LoadInventory::GetUserCreature, get_int(9) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            // ORIG 0x0840d1ec：byte3 = stat；随后 `movb $0x0,-0x161` 是 lockKey
            // 的声明处清零（0x0840d1f2），故 lockKey 声明移到此处。
            detailWord[3] = static_cast<unsigned char>(stat);
            unsigned char lockKey = 0;
            if (!db->get_ubyte(10, lockKey))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x2880, 5)
                    ("DB_LoadInventory::GetUserCreature, get_ubyte(10) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (!db->get_int(11, rowBuf.expire))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x28a6, 5)
                    ("DB_LoadInventory::GetUserCreature, get_int(12) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (0xf1 < rowBuf.slot)
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x28ad, 5)
                    ("DB_LoadInventory::GetUserCreature, slot_no overflow ERROR"
                     " charac_no=%u, slot_no=%d", sig->m_characNo, rowBuf.slot);
                return 0;
            }
            // [本波实测] `if (dupSlot != 0) 栈副本 else 主槽` 翻转形：opcode
            // 匹配 681→677 且差异行 112→120（4.4 仍产 `jne`+主槽直落）⇒ 回滚，
            // 维持下方 `if (dupSlot == 0) 主槽 else 栈副本`（N122 族封顶）。
            if (dupSlot == 0)
            {
                // 主槽位（m_invenType3，步长 0x3d）。GetUserCreature.md 实证
                // +0x02/+0x07 为两条独立 movl（4B），Ghidra 的 *(ulong*) 8B
                // 表达为伪影 —— 原 (unsigned long long*) 写为宽度错误，修正。
                Inven_Item& item = sig->m_invenType3[rowBuf.slot];
                item.m_field1 = Inven_Item::ITEM_TYPE_CREATURE_EQUIP;  // +0x01 movb 5
                item.m_fieldb = rowBuf.endurance;                       // +0x0b movw
                item.m_addInfo = static_cast<int>(rowBuf.itId);         // +0x02 movl
                item.m_addInfo2 = rowBuf.uiId;                          // +0x07 movl
                item.m_amp.SetLock(lockKey);                            // +0x11 r/w
            }
            else
            {
                // ORIG 0x0840d343 `lea -0x1a5(%ebp),%eax; call Inven_Item::C1Ev`
                // （W 0x080cb854）之后**紧跟**独立 memset：0x0840d351
                // `movl $0x3d,0x8(%esp)` + 0x0840d359 `movl $0x0,0x4(%esp)` +
                // 0x0840d361 `lea -0x1a5(%ebp),%eax` + 0x0840d367 `mov %eax,(%esp)`
                // + 0x0840d36a `call memset@plt` ⇒ 原码 ctor 后确有
                // `memset(&item, 0, 0x3d)`（0x3d = sizeof(Inven_Item)，ORIG 立即数）。
                // [Round7 FillC] 旧判词「ctor 内 reset 已清全字段 ⇒ 无独立 memset」
                // 与本直证矛盾 ⇒ 回滚删除（P171：ORIG 死写入照抄；p167 调用点差集
                // 该 memset 为 MISSING）。
                Inven_Item item;
                memset(&item, 0, 0x3d);
                item.m_field1 = Inven_Item::ITEM_TYPE_CREATURE_EQUIP;
                item.m_fieldb = rowBuf.endurance;
                item.m_addInfo = static_cast<int>(rowBuf.itId);   // +0x02 movl 4B（ORIG 栈副本同构）
                item.m_addInfo2 = rowBuf.uiId;
                item.m_amp.SetLock(lockKey);
                dupSlot = 0;
                overflow.push_back(item);
            }
            // 详情条目（基址 +0x12880 = m_creature，步长 0x38；ORIG 全 14 条
            // movl 升序写：+0x08..0x18 为 name 16B 四 dword，其中后 4B 落
            // m_pad14；+0x1c/+0x2c/+0x30/+0x34 为清 0 字段）。
            SIG_CREATURE_ITEM_DETAIL_INFO* d =
                reinterpret_cast<SIG_CREATURE_ITEM_DETAIL_INFO*>(sig->m_creature)
                + row;
            d->m_slotNo = rowBuf.slot;                       // +0x00
            d->m_uid = rowBuf.uiId;                          // +0x04
            *reinterpret_cast<int*>(&d->m_name[0])  = *reinterpret_cast<int*>(&rowBuf.name[0]);
            *reinterpret_cast<int*>(&d->m_name[4])  = *reinterpret_cast<int*>(&rowBuf.name[4]);
            *reinterpret_cast<int*>(&d->m_name[8])  = *reinterpret_cast<int*>(&rowBuf.name[8]);
            *reinterpret_cast<int*>(&d->m_pad14[0]) = *reinterpret_cast<int*>(&rowBuf.name[12]);
            d->m_stomachValue = rowBuf.stomach;              // +0x18
            *reinterpret_cast<int*>(&d->m_pad1c[0]) = rowBuf.m_field48;   // +0x1c
            d->m_exp = rowBuf.exp;                           // +0x20
            *reinterpret_cast<int*>(&d->m_creatureItemType) =
                *reinterpret_cast<int*>(&detailWord[0]);     // +0x24..0x27 四字节
            d->m_expireTime = rowBuf.expire;                 // +0x28
            *reinterpret_cast<int*>(&d->m_pad2c[0]) = rowBuf.m_field58;   // +0x2c
            *reinterpret_cast<int*>(&d->m_growExpFlag) = rowBuf.m_field5c; // +0x30
            *reinterpret_cast<int*>(&d->m_pad32[2]) = rowBuf.m_field60;    // +0x34
        }
    }

    // ---- overflow vector：填首个空槽并触发 SwapCreatureItem ----
    // b28d-167/28：ORIG 0x0840ca74 的 p167 差集 = MISSING operator*(deEv)×4 +
    // operator++(int)(ppEi)×1，EXTRA operator->(ptEv)×4 + operator++(ppEv)×1
    // ⇒ 源码须写 `(*it).field` 与后置 `it++`（P167/P28）。
    int emptySlot = 0;
    for (std::vector<Inven_Item>::iterator it = overflow.begin();
         it != overflow.end(); it++)
    {
        for (; emptySlot < 0x8d; ++emptySlot)
        {
            // ORIG 0x0840d556/0x0840d55e/0x0840d563：`movzbl used[emptySlot]; xor $1;
            // test %al; je <++emptySlot 段>` ⇒ 同处 A 的 `if (!used[...])` 形。
            if (!used[emptySlot])
            {
                used[emptySlot] = 1;
                if (emptySlot == 0x8c)
                {
                    emptySlot = 0xee;
                }
                break;
            }
        }
        int cur = emptySlot;
        if (cur != 0xee && cur > 0x8b)
        {
            cMyTrace("bool DB_LoadInventory::GetUserCreature("
                     "SIG_LOAD_INVENTORY*)", 0x28e9, 5)
                ("DB_LoadInventory::GetUserCreature, charac_no=%u,"
                 " slot overflow", sig->m_characNo);
            break;
        }
        Inven_Item& dst = sig->m_invenType3[cur];
        dst.m_field1 = Inven_Item::ITEM_TYPE_CREATURE_EQUIP;
        dst.m_fieldb = (*it).m_fieldb;                     // +0x0b movw
        dst.m_addInfo = (*it).m_addInfo;                   // +0x02 movl（4B，ORIG）
        dst.m_addInfo2 = (*it).m_addInfo2;                 // +0x07 movl
        dst.m_amp.SetLock((*it).m_amp.GetLock());          // +0x11
        DB_SwapCreatureItem::makeRequest(dst.m_addInfo2, cur);
    }
    return 1;
}