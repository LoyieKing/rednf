int importPremiumList(const char* path,
                      stPremiumTable_t* table,
                      std::vector<SPremiumOverEquipableInfo>& overVec,
                      std::vector<SPremiumReturnItemInfo>& returnVec)
{
    if (!loadRDARScriptFile("", path))
        return 0x14a6;

    std::string token;
    std::string value;
    // [b28d-solo-CUserPremium] P171/P232：ORIG +0x45 `movl $0x0,-0x6c(%ebp)`
    // 全函数唯一一次写、无读（死局部占帧）⇒ 照抄。
    int deadLocal = 0;
    bool ok = false;
    // [b28d-solo-CUserPremium] 槽序对齐试形：ORIG cur=-0x68 深于
    // equipCase=-0x60（4.1 先声明更深）⇒ 把 cur 的**声明**提到 equipCase
    // 之前（无初始化），赋值仍留在 clean 循环之后（ORIG +0x7c 才写 cur）。
    stPremiumTable_t* cur;
    // ORIG i=-0x64 介于 cur(-0x68) 与 equipCase(-0x60) 之间 ⇒ 同法提头。
    int i;
    // ORIG -0x60 槽：跨组保留的标签分类值（未知标签时写入残留值）；ORIG 头段
    // 无 `movl $0,-0x60` ⇒ 原码声明处不带初始化。
    int equipCase;

    for (i = 0; i <= 0x69; ++i)
        table[i].clean();

    cur = table;

    while (true)
    {
        if (!ScanType(token, true))
            return 0;

        if (token == "[overequip list]")
        {
            SPremiumOverEquipableInfo elem;
            memset(&elem, 0, sizeof(elem));
            overVec.push_back(elem);
            while (true)
            {
                elem.m_count = ScanInt(&ok);
                if (!ok)
                    break;
                for (int j = 0; elem.m_count > j; ++j)
                {
                    if (!ScanStr(&value))
                        return 0x14c5;
                    if (value == "[hat avatar]")          equipCase = 0x0;
                    else if (value == "[hair avatar]")     equipCase = 0x1;
                    else if (value == "[face avatar]")     equipCase = 0x2;
                    else if (value == "[coat avatar]")     equipCase = 0x3;
                    else if (value == "[pants avatar]")    equipCase = 0x4;
                    else if (value == "[shoes avatar]")    equipCase = 0x5;
                    else if (value == "[breast avatar]")   equipCase = 0x6;
                    else if (value == "[waist avatar]")    equipCase = 0x7;
                    else if (value == "[skin avatar]")     equipCase = 0x8;
                    else if (value == "[aurora avatar]")   equipCase = 0x9;
                    else if (value == "[weapon]")          equipCase = 0xa;
                    else if (value == "[title name]")      equipCase = 0xb;
                    else if (value == "[coat]")            equipCase = 0xc;
                    else if (value == "[shoulder]")        equipCase = 0xd;
                    else if (value == "[pants]")           equipCase = 0xe;
                    else if (value == "[shoes]")           equipCase = 0xf;
                    else if (value == "[waist]")           equipCase = 0x10;
                    else if (value == "[amulet]")          equipCase = 0x11;
                    else if (value == "[wrist]")           equipCase = 0x12;
                    else if (value == "[ring]")            equipCase = 0x13;
                    else if (value == "[support]")         equipCase = 0x14;
                    else if (value == "[magic stone]")     equipCase = 0x15;
                    elem.m_slots[j] = (ENUM_OVER_EQUIP_SLOT)equipCase;
                }
                overVec.push_back(elem);
            }
            continue;
        }

        if (token == "[returnitem list]")
        {
            // [b28d-solo-CUserPremium] P171/P232 死局部照抄：ORIG 0x08906221
            // `movl $0x0,-0x58(%ebp)`（memset 之前）是全函数对该槽唯一一次
            // 访问，之后无读（grep -0x58(%ebp) 仅此一条）⇒ 原码分支头遗留
            // 的未使用 int 局部；缺它少 1 条指令。
            int deadSlot = 0;
            SPremiumReturnItemInfo elem;
            memset(&elem, 0, sizeof(elem));
            returnVec.push_back(elem);
            while (true)
            {
                elem.m_count = ScanInt(&ok);
                if (!ok)
                    break;
                for (int k = 0; elem.m_count > k; ++k)
                {
                    elem.m_items[k] = ScanInt(&ok);
                    if (!ok)
                        return 0x1512;
                }
                returnVec.push_back(elem);
            }
            continue;
        }

        if (token == "[type]")
        {
            int idx = ScanInt((bool*)0);
            if (idx > 0x69)
                return 0x151b;
            cur = table + idx;
            continue;
        }

        if (token == "[name]")
        {
            if (!ScanStr(&value))
                return 0x1520;
            cur->m_name = value;
            continue;
        }

        if (token == "[attr]")
        {
            if (!ScanStr(&value))
                return 0x1525;
            if (value == "[term]")
            {
                cur->m_kind = 2;
                continue;
            }
            if (value == "[bonus]")
            {
                cur->m_kind = 1;
                continue;
            }
            return 0x152b;
        }

        if (token == "[term]")
        {
            cur->m_checkTerm = ScanInt((bool*)0);
            continue;
        }

        if (token == "[expire time]")
        {
            cur->m_expireTime = ScanInt((bool*)0);
            continue;
        }

        if (token == "[items]")
        {
            // [b28d-solo-CUserPremium] P0/P198①：ORIG 0x0890646f 分支头三连
            // `movl $0x0,-0x4c/-0x48/-0x44`（itemIdx/cnt/n 三个具名局部声明即
            // 置零、循环内仅赋值）⇒ 提至分支头、循环体只赋值。
            int itemIdx = 0;
            int cnt = 0;
            int n = 0;
            for (;;)
            {
                itemIdx = ScanInt(&ok);
                if (!ok)
                    break;
                cnt = ScanInt(&ok);
                if (!ok)
                    break;
                cur->m_advantageItems[n].first = itemIdx;
                cur->m_advantageItems[n].second = cnt;
                ++n;
            }
            continue;
        }

        if (token == "[fatigue]")
        {
            cur->m_fatigueRate = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[unlimit fatigue]")
        {
            cur->m_unlimitFatigue = (ScanInt((bool*)0) == 1);
            continue;
        }

        if (token == "[bonus exp]")
        {
            cur->m_bonusExp = ScanInt(&ok);
            if (!ok)
                return 0;  // ORIG 怪癖（0x8906f2d）：此处扫描失败按成功返回 0
            continue;
        }

        if (token == "[quest item drop rate]")
        {
            cur->m_questItemDropRate = ScanInt(&ok);
            if (!ok)
                return 0;  // 同上怪癖
            continue;
        }

        if (token == "[independent drop rate]")
        {
            // [b28d-solo-CUserPremium] P0/P211：ORIG +0x842 rate 局部为 **int**
            // （for-init `movl $0x0,-0x40` 在头、for-incr `movl $0x0,-0x40` 在尾
            // +0x883）；push_back 处 `mov -0x40,%eax; mov %eax,-0x70` 转换临时
            // ⇒ 原码 `push_back(rate)` 由 int→unsigned int 产生右值临时，选中
            // `push_back(unsigned int&&)`（EOj）。旧 `unsigned int rate` +
            // static_cast<unsigned int&&> 无 -0x70 临时且尾无 re-init。
            for (int rate = 0; ; rate = 0)
            {
                rate = ScanInt(&ok);
                if (!ok)
                    break;
                cur->m_independentDropRates.push_back(rate);
            }
            continue;
        }

        if (token == "[exp]")
        {
            cur->m_expRate = ScanInt((bool*)0);
            continue;
        }

        if (token == "[coin]")
        {
            cur->m_coin = ScanInt((bool*)0);
            continue;
        }

        if (token == "[gold bonus]")
        {
            ScanStr(&value);  // ORIG 不检查返回值（0x890669c 无判空）
            if (value == "[true]")
            {
                cur->m_goldBonus = 1;
                continue;
            }
            if (value == "[false]")
            {
                cur->m_goldBonus = 0;
                continue;
            }
            return 0x156d;
        }

        if (token == "[over skill]")
        {
            cur->m_overSkillLevel = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[cerashop discount rate]")
        {
            cur->m_cerashopDiscountRate = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[apply target server]")
        {
            if (!ScanStr(&value))
                return 0x1583;
            if (value == "self")
            {
                cur->m_useTargetServer = 1;
                continue;
            }
            if (value == "all")
            {
                cur->m_useTargetServer = 0;
                continue;
            }
            return 0x1588;
        }

        if (token == "[over equipinfo]")
        {
            // [b28d-solo-CUserPremium] P212/P198①：ORIG 0x089067f7..0x08906805
            // 分支头三连置零 `movl $0x0,-0x3c/-0x38/-0x34`，且 -0x3c 全函数
            // 仅此一次写、无读（P171/P232 死局部）；-0x38/-0x34 为 cnt/itemId
            // （声明即置零、循环内仅赋值）。旧形把 itemId/cnt 写在循环内直接
            // 初始化，缺三条置零指令。
            int deadSlot2 = 0;
            int cnt = 0;
            int itemId = 0;
            cur->m_overEquipCount = ScanInt((bool*)0);
            if (cur->m_overEquipCount > 5)
                return 0x1591;
            for (int m = 0; cur->m_overEquipCount > m; ++m)
            {
                itemId = ScanInt(&ok);
                if (!ok)
                    break;
                cnt = ScanInt(&ok);
                if (!ok)
                    break;
                cur->m_overEquipPairs[m].first = itemId;
                cur->m_overEquipPairs[m].second = cnt;
            }
            continue;
        }

        if (token == "[return iteminfo]")
        {
            // [b28d-solo-CUserPremium] P212/P198①：ORIG 0x089068cb..0x089068d9
            // 分支头三连置零 `movl $0x0,-0x2c/-0x28/-0x24`（slot/cnt/itemId）
            // **先于** 0x089068e0 的 ScanInt(m_returnItemCount)；旧形在循环内
            // 直接初始化 itemId/cnt，缺三条置零。
            int slot = 0;
            int cnt = 0;
            int itemId = 0;
            cur->m_returnItemCount = ScanInt((bool*)0);
            if (cur->m_returnItemCount > 3)
                return 0x15a6;
            for (int n = 0; cur->m_returnItemCount > n; ++n)
            {
                if (slot > 2)
                    return 0x15aa;
                itemId = ScanInt(&ok);
                if (!ok)
                    break;
                cnt = ScanInt(&ok);
                if (!ok)
                    break;
                cur->m_returnItemPairs[slot].first = itemId;
                cur->m_returnItemPairs[slot].second = cnt;
                ++slot;
            }
            continue;
        }

        if (token == "[apply date]")
        {
            // ORIG 0x089069bc d=-0x18、0x089069ce v=-0x1c（4.1 先声明更深）；
            // 4.4 按 for-init 先声明给 d 更深槽 ⇒ 把 v 提到循环外先声明
            // （无初始化，ORIG 循环头无 v 置零），d 留在 for init。
            int v;
            for (int d = 0;; ++d)
            {
                v = ScanInt(&ok);
                if (!ok)
                    break;
                cur->m_applyDayMask[d] = v;
            }
            continue;
        }

        if (token == "[HP MAX]")
        {
            cur->m_hpMax = ScanInt((bool*)0);
            continue;
        }

        if (token == "[MP MAX]")
        {
            cur->m_mpMax = ScanInt((bool*)0);
            continue;
        }

        if (token == "[physical attack]")
        {
            cur->m_physicalAttack = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[physical defense]")
        {
            cur->m_physicalDefense = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[magical attack]")
        {
            cur->m_magicalAttack = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[magical defense]")
        {
            cur->m_magicalDefense = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[fire resistance]")
        {
            cur->m_fireResistance = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[water resistance]")
        {
            cur->m_waterResistance = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[dark resistance]")
        {
            cur->m_darkResistance = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[light resistance]")
        {
            cur->m_lightResistance = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[inventory limit]")
        {
            cur->m_inventoryLimit = ScanInt((bool*)0);
            continue;
        }

        if (token == "[HP regen speed]")
        {
            cur->m_hpRegenSpeed = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[MP regen speed]")
        {
            cur->m_mpRegenSpeed = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[move speed]")
        {
            cur->m_moveSpeed = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[attack speed]")
        {
            cur->m_attackSpeed = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[cast speed]")
        {
            cur->m_castSpeed = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[hit recovery]")
        {
            cur->m_hitRecovery = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[jump power]")
        {
            cur->m_jumpPower = (unsigned short)ScanInt((bool*)0);
            continue;
        }

        if (token == "[target item]")
        {
            // [b28d-solo-CUserPremium] P0：ORIG 0x08906f1a 该分支自带
            // `movl $0x0,-0x14` + `movb $0x0,-0x7a` 两槽（int v 与分支局部
            // bool），与 [item] 分支的 -0x7b bool 各占独立槽——原码是两个
            // 分支各自的局部，非函数头单例 itemOk。
            int v = 0;
            bool itemOk = false;
            v = ScanInt(&itemOk);
            if (!itemOk)
                return 0;  // ORIG 怪癖（0x8906f2c）：失败按成功返回 0
            cur->m_targetItemKey = (unsigned int)v;
            continue;
        }

        if (token == "[item]")
        {
            // [b28d-solo-CUserPremium] P0：ORIG 0x08906f0a `movb $0x0,-0x7b`
            // 先于 item 的 C1（0x08906f0f），且循环内 [term]/[restrict] 复用
            // 同一槽 ⇒ 分支局部 bool 声明在 item 之前。
            bool itemOk = false;
            premium_script_parsing::STPremiumItemData item;
            item.m_key = (unsigned int)ScanInt(&itemOk);
            if (!itemOk)
                return 0x15fb;
            while (true)
            {
                if (!ScanType(token, true))
                    return 0x1600;
                if (token == "[term]")
                {
                    // [b28d-solo-CUserPremium] 真缺口修正：ORIG 0x08906e77..
                    // 0x08906ea3 是**分支内循环**——成功写字段后 `jmp 08906e77`
                    // 回分支头继续 ScanInt，失败才 `jmp 08906e3c` 回 ScanType。
                    // 旧单次形（处理一个值就 continue 回 ScanType）在
                    // 「[term] v1 v2」或连续多个 [term] 的脚本上语义不同
                    // （多消费一次 ScanType / 丢后续值）。Ghidra C 段同证：
                    // `while(1){ local_14=0; local_14=ScanInt(&ok); if(!ok) break;
                    //  item.m_termFlag=(uchar)local_14; }`。
                    while (true)
                    {
                        int v = 0;
                        v = ScanInt(&itemOk);
                        if (!itemOk)
                            break;
                        item.m_termFlag = (unsigned char)v;
                    }
                    continue;
                }
                if (token == "[restrict]")
                {
                    // ORIG 0x08906ebc..0x08906ee2：与 [term] 同构的内循环
                    // （成功 `jmp 08906ebc` 回分支头，失败 `jmp 08906e3c`）。
                    while (true)
                    {
                        int v = 0;
                        v = ScanInt(&itemOk);
                        if (!itemOk)
                            break;
                        item.m_restrictFlag = (unsigned char)v;
                    }
                    continue;
                }
                if (token == "[/item]")
                {
                    cur->m_premiumItems.push_back(item);
                    break;
                }
                // 其余 token：继续扫描（ORIG 8906f02 → 重入 ScanType）
            }
            continue;
        }

        // 未知顶层 token：跳过继续（ORIG 比较链全部落空 → 回循环头）
    }
