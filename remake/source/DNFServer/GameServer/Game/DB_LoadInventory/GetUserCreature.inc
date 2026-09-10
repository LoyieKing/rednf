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
    bool dupSlot = false;
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
        // ORIG 以 memset(local_220,0,0x74) 合并清零下列局部变量，语义等价。
        char name[16];
        unsigned long itId = 0;
        unsigned short endurance = 0;
        int slot = 0, uiId = 0, stomach = 0, exp = 0;
        int creatureType = 0, noCharge = 0, stat = 0, expire = 0;
        unsigned char lockKey = 0;
        memset(name, 0, sizeof(name));

        ok = db->get_int(0, slot);
        // dup 检测在 get_int 失败检查之前（ORIG 0x282c 出口顺序）。
        // ORIG 0840cc9f 段：`mov slot,%eax; mov %eax,-0xc; cmpl $0xee; jne;
        // movl $0x8c,-0xc; cmpl $0x8c,-0xc; jle` ⇒ 原码是「先赋值再条件
        // 覆盖」的 if 形（非三目），且判定为 `<= 0x8c`（jle 正向）。
        int checkSlot = slot;
        if (checkSlot == 0xee)
        {
            checkSlot = 0x8c;
        }
        if (checkSlot <= 0x8c)
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
                dupSlot = true;
            }
            if (!ok)
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x282c, 5)
                    ("DB_LoadInventory::GetUserCreature, get_int(0) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (!db->get_ulong(1, itId))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x2833, 5)
                    ("DB_LoadInventory::GetUserCreature, get_ushort(1) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (!db->get_str(2, name, 0xd))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x2846, 5)
                    ("DB_LoadInventory::GetUserCreature, get_int(2) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (!db->get_int(3, stomach))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x284e, 5)
                    ("DB_LoadInventory::GetUserCreature, get_int(3) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (!db->get_int(4, exp))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x2855, 5)
                    ("DB_LoadInventory::GetUserCreature, get_int(4) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (!db->get_ushort(5, endurance))
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
            if (!db->get_int(7, uiId))
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
            if (!db->get_int(9, stat))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x2878, 5)
                    ("DB_LoadInventory::GetUserCreature, get_int(9) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (!db->get_ubyte(10, lockKey))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x2880, 5)
                    ("DB_LoadInventory::GetUserCreature, get_ubyte(10) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (!db->get_int(11, expire))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x28a6, 5)
                    ("DB_LoadInventory::GetUserCreature, get_int(12) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (0xf1 < slot)
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x28ad, 5)
                    ("DB_LoadInventory::GetUserCreature, slot_no overflow ERROR"
                     " charac_no=%u, slot_no=%d", sig->m_characNo, slot);
                return 0;
            }
            // detailWord 字节布局（SIG_CREATURE_ITEM_DETAIL_INFO +0x24）：
            // [0]=creature_type [1]=0 [2]=no_charge [3]=stat。
            int detailWord = static_cast<unsigned char>(creatureType);
            detailWord |= static_cast<unsigned char>(noCharge) << 16;
            detailWord |= static_cast<unsigned char>(stat) << 24;

            if (!dupSlot)
            {
                // 主槽位（m_invenType3，步长 0x3d）。GetUserCreature.md 实证
                // +0x02/+0x07 为两条独立 movl（4B），Ghidra 的 *(ulong*) 8B
                // 表达为伪影 —— 原 (unsigned long long*) 写为宽度错误，修正。
                Inven_Item& item = sig->m_invenType3[slot];
                item.m_field1 = Inven_Item::ITEM_TYPE_CREATURE_EQUIP;  // +0x01 movb 5
                item.m_fieldb = endurance;                              // +0x0b movw
                item.m_addInfo = static_cast<int>(itId);                // +0x02 movl
                item.m_addInfo2 = uiId;                                 // +0x07 movl
                item.m_amp.SetLock(lockKey);                            // +0x11 r/w
            }
            else
            {
                Inven_Item item;
                memset(&item, 0, sizeof(item));
                item.m_field1 = Inven_Item::ITEM_TYPE_CREATURE_EQUIP;
                item.m_fieldb = endurance;
                item.m_addInfo = static_cast<int>(itId);   // +0x02 movl 4B（ORIG 栈副本同构）
                item.m_addInfo2 = uiId;
                item.m_amp.SetLock(lockKey);
                dupSlot = false;
                overflow.push_back(item);
            }
            // 详情条目（基址 +0x12880 = m_creature，步长 0x38；ORIG 全 14 条
            // movl 升序写：+0x08..0x18 为 name 16B 四 dword，其中后 4B 落
            // m_pad14；+0x1c/+0x2c/+0x30/+0x34 为清 0 字段）。
            SIG_CREATURE_ITEM_DETAIL_INFO* d =
                reinterpret_cast<SIG_CREATURE_ITEM_DETAIL_INFO*>(sig->m_creature)
                + row;
            d->m_slotNo = slot;                              // +0x00
            d->m_uid = uiId;                                 // +0x04
            *reinterpret_cast<int*>(&d->m_name[0])  = *reinterpret_cast<int*>(&name[0]);
            *reinterpret_cast<int*>(&d->m_name[4])  = *reinterpret_cast<int*>(&name[4]);
            *reinterpret_cast<int*>(&d->m_name[8])  = *reinterpret_cast<int*>(&name[8]);
            *reinterpret_cast<int*>(&d->m_pad14[0]) = *reinterpret_cast<int*>(&name[12]);
            d->m_stomachValue = stomach;                     // +0x18
            *reinterpret_cast<int*>(&d->m_pad1c[0]) = 0;     // +0x1c
            d->m_exp = exp;                                  // +0x20
            *reinterpret_cast<int*>(&d->m_creatureItemType) =
                detailWord;                                  // +0x24..0x27 四字节
            d->m_expireTime = expire;                        // +0x28
            *reinterpret_cast<int*>(&d->m_pad2c[0]) = 0;     // +0x2c
            *reinterpret_cast<int*>(&d->m_growExpFlag) = 0;  // +0x30
            *reinterpret_cast<int*>(&d->m_pad32[2]) = 0;     // +0x34
        }
        else
        {
            cMyTrace("bool DB_LoadInventory::GetUserCreature("
                     "SIG_LOAD_INVENTORY*)", 0x281d, 5)
                ("DB_LoadInventory::GetUserCreature, charac_no=%u, slot=%d"
                 " overflow", sig->m_characNo, checkSlot);
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
