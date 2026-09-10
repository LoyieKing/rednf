bool importUpgradeTable(const char* path, upgrade_table_t* table)
{
    // [A16 round5 修复 + 判词] 本件含一处**真语义修**：12 处把 int 视角
    //   `m_prob00[k] / m_w38[4] / m_w4c[j] / m_w5c[j]` 改成同址 float 视角
    //   （m_f00[0,1,5,2,6,3]/m_f38[4]/m_f4c[1]/m_f5c[0]/m_f5c[1]/m_f4c[2]/
    //   m_f5c[2]/m_f4c[3]），因 ORIG 该 12 处为 `fildl … fstps`（浮点存储）：
    //   int 视角下 4.4 恒折 fnstcw/movzwl/fldcw×2/fistpl/mov 六条往返（12 处
    //   共 -95 条），且**运行期把 float 位模式当整数截断**（下游 CItemAmplifier.cpp
    //   0x0854b282 段以 m_f00/m_f4c 视图读取）⇒ 原为真 bug。
    //   fresh 读数：改前 fistpl 12/fnstcw 12/fldcw 24/fstps 8，改后 fildl 2/fstps 20
    //   = ORIG 完全一致；gap 1167→1072（ORIG 1119：+48 → -47）。
    //   残余 47 条 = 帧/槽分区：ORIG `sub $0x190` vs 我方 `$0x160`（试补
    //   char[0x30] 死局部可让帧回 0x190，但槽号不动、gap 与四数零变化 ⇒ 已回退）
    //   + 槽号体系差（ORIG ok@-0xc6 vs 我方 -0x95）⇒ N82/N126 版本常数封顶；
    //   p167 仅别名伪差（C1/C2·D1/D2 同址，P93）+ _Unwind_Resume ×1（我方多一处
    //   EH landing）。语义等价（token 比较方向/表插入/错误路径均与 ORIG 对齐）。
    // [轴 B 2026-09-10 · a_S->U 清单件 = **N（表内配对假阳性）**] 清单行
    //   `orig_addr=0x08901016 setle` 与我方 `setbe` 系按出现序号交叉配对。按地址自校：
    //   ORIG 0x08901016 = `addl $0x1,-0x74(%ebp); cmpl $0x4,-0x74(%ebp); setle`
    //   （有符号；字段循环尾测）↔ 我方 0x390b 同构 setle（已一致）；
    //   我方唯一 setbe（0x3895 `call vector<upgrade_info_t>::size(); cmp -0x58(%ebp),%eax;
    //   setbe`）对应 ORIG 0x08901039 `call size(); cmp -0x7c(%ebp),%eax; jbe`
    //   —— 两侧同为**无符号序**且上下文同构 ⇒ 本就一致，**禁改**（P196）。
    //   实测该件 1119/1074（gap 45）不变，未动一行源码。
    if (!loadRDARScriptFile("", path))
    {
        return 0;
    }

    try
    {
        std::string token;
        std::string rarityName;
        bool ok = false;

        for (;;)
        {
            if (!ScanType(token, true))
            {
                return 1;                               // EOF → 成功
            }

            if (token == "[table]")
            {
                table->m_infos.clear();
                for (;;)
                {
                    upgrade_info_t info;
                    info.m_f00[0] =
                        static_cast<float>(ScanInt(&ok));
                    if (!ok)
                    {
                        break;
                    }
                    // [A16 round5 真语义修] 报告 0x08900d13 实证：本行在首字段
                    // 检查之后即清 +0x10 = `mov $0x0,%eax; mov %eax,-0x154`（float
                    // 视角两段形 ⇒ 0.0f），不是 [table] 段末尾的 int 单条 movl。
                    info.m_f00[4] = 0.0f;
                    info.m_f00[1] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    info.m_f00[5] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    info.m_f00[2] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    info.m_f00[6] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    info.m_f00[3] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    // [A16 round5 真语义修] 报告 0x08900de1 `mov $0x0,%eax; mov
                    // %eax,-0x148`（+0x1c = 0.0f 两段形）⇒ m_f00[7] 是常量 0，
                    // **不消费** ScanInt。原写 `m_prob00[7] = ScanInt(&ok)` 使其后
                    // 5 个整数整体错位一槽（m_prob20[2]/m_recipeItem 取值错、
                    // m_meterialCount 恒 0）—— 真 bug。
                    info.m_f00[7] = 0.0f;
                    info.m_prob20[0] = ScanInt(&ok);
                    info.m_prob20[1] = ScanInt(&ok);
                    info.m_prob20[2] = ScanInt(&ok);
                    info.m_recipeItem = ScanInt(&ok);
                    // 报告 0x08900e45 (→+0x30) 为第 5 个连续 ScanInt，0x08900e50
                    // (→+0x34) 才是 `movl $0x0`（m_discountBase = 0，int 单条）。
                    info.m_meterialCount = ScanInt(&ok);
                    info.m_discountBase = 0;
                    info.m_f38[4] =
                        static_cast<float>(ScanInt(&ok));
                    if (!ok)
                    {
                        break;
                    }
                    info.m_f5c[0] = 0.0f;
                    info.m_f4c[1] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    info.m_f5c[1] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    info.m_f4c[2] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    info.m_f5c[2] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    info.m_f4c[3] = ScanFloat(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    table->m_infos.push_back(info);
                }
            }
            else if (token == "[amplification const]")
            {
                size_t index = 0;
                ok = true;                    // ORIG local_cb = true（入循环前置真）
                for (;;)
                {
                    // ORIG 条件本体是 `ok == false || size() <= index` 的合并短路
                    // （0x08901020 起 + bool 汇聚岛）。ok 复位为 true 的语义已补（下方）；
                    // 汇聚岛在 4.4 下改命名 bool 反而多 12 insn（实测 1165→1177），
                    // 故保留直判形，岛形差记 N3/setcc-fold。
                    if (index >= table->m_infos.size())
                    {
                        break;
                    }
                    upgrade_info_t& info = table->m_infos[index];
                    ++index;
                    for (int i = 0; i <= 4; ++i)
                    {
                        if (i == 0)
                        {
                            info.m_w38[i] = 0;
                        }
                        else
                        {
                            info.m_w38[i] = ScanInt(&ok);
                        }
                        if (!ok)
                        {
                            break;
                        }
                    }
                }
                if (!ScanType(token, true))
                {
                    LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                                          __FUNCTION__, 0xb3a,
                                          "ERROR [amplification const] count %d",
                                          static_cast<int>(index));
                }
            }
            else if (token == "[cost weights by rarity]")
            {
                for (int i = 0; i <= 5; ++i)
                {
                    UPG_TBL_F32(table, i + 0x1b) = ScanFloat(&ok);
                    if (!ok)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                            0xb48, "importUpgradeTable - invalid cost rarity table");
                    }
                }
            }
            else if (token == "[stat weights by rarity]")
            {
                for (int i = 0; i <= 5; ++i)
                {
                    UPG_TBL_F32(table, i + 0x20) = ScanFloat(&ok);
                    if (!ok)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                            0xb5a, "importUpgradeTable - invalid stat rarity table");
                    }
                }
            }
            else if (token == "[type]")
            {
                for (int i = 10; i <= 0x15; ++i)
                {
                    UPG_TBL_F32(table, i + 0x4) = ScanFloat(&ok);
                    if (!ok)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                            0xb6c, "importUpgradeTable - invalid type table");
                    }
                }
            }
            else if (token == "[level]")
            {
                for (int i = 0; i <= 3; ++i)
                {
                    UPG_TBL_F32(table, i) = ScanFloat(&ok);
                    if (!ok)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                            0xb7e, "importUpgradeTable - invalid level table");
                    }
                }
            }
            else if (token == "[cost]")
            {
                for (int i = 0; i <= 0x63; ++i)
                {
                    UPG_TBL_I32(table, i + 0x26) = ScanInt(&ok);
                    if (!ok)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                            0xb90, "importUpgradeTable - invalid gold table");
                    }
                }
            }
            else if (token == "[notice]")
            {
                UPG_TBL_I32(table, 0x8a) = ScanInt(&ok);
                if (!ok)
                {
                    LogManager::logFormat(
                        0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                        0xb9e, "importUpgradeTable - invalid notice tag");
                }
            }
            else if (token == "[repair cost rate by upgrade level]")
            {
                for (int i = 0; i <= 0x63; ++i)
                {
                    UPG_TBL_F32(table, i + 0x9b) = ScanFloat(&ok);
                    if (!ok)
                    {
                        if (i != 0x63)
                        {
                            LogManager::logFormat(
                                0, "../RDARScriptEtcParameter.cpp",
                                __FUNCTION__, 0xbac,
                                "ERROR : [repair cost rate by upgrade level] size %d / %d",
                                i, 100);
                        }
                        break;
                    }
                }
            }
            else if (token == "[destroy level by rarity]")
            {
                while (ScanStr(&rarityName))
                {
                    int level = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0;
                    }
                    int rarity = getItemRarity(rarityName.c_str());
                    if (rarity == 6)
                    {
                        return 0;
                    }
                    UPG_TBL_I32(table, rarity + 0xfe) = level;
                }
            }
            else if (token == "[max upgrade level by rarity]")
            {
                while (ScanStr(&rarityName))
                {
                    int level = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0;
                    }
                    int rarity = getItemRarity(rarityName.c_str());
                    if (rarity == 6)
                    {
                        return 0;
                    }
                    UPG_TBL_I32(table, rarity + 0x104) = level;
                }
            }
            else if (token == "[disjoint]")
            {
                for (;;)
                {
                    if (!ScanType(token, true))
                    {
                        throw static_cast<unsigned int>(0xbdc);
                    }
                    if (token == "[/disjoint]")
                    {
                        break;
                    }
                    if (token == "[disjoint bonus item]")
                    {
                        UPG_TBL_I32(table, 0x8b) = ScanInt(&ok);
                        if (!ok)
                        {
                            break;
                        }
                    }
                    else if (token == "[equip level const]")
                    {
                        UPG_TBL_I32(table, 0x8c) = ScanInt(&ok);
                        if (!ok)
                        {
                            throw static_cast<unsigned int>(0xbea);
                        }
                    }
                    else if (token == "[upgrade failed bonus weight by rarity]")
                    {
                        while (ScanStr(&rarityName))
                        {
                            float weight = ScanFloat(&ok);
                            if (!ok)
                            {
                                throw static_cast<unsigned int>(0xbf4);
                            }
                            int rarity = getItemRarity(rarityName.c_str());
                            if (rarity == 6)
                            {
                                throw static_cast<unsigned int>(0xbf7);
                            }
                            UPG_TBL_F32(table, rarity + 0x8d) = weight;
                        }
                    }
                    else if (token == "[upgrade const for bonus item count]")
                    {
                        UPG_TBL_I32(table, 0x93) = ScanInt(&ok);
                        if (!ok)
                        {
                            throw static_cast<unsigned int>(0xbff);
                        }
                    }
                    else if (token == "[correction grade by rarity]")
                    {
                        while (ScanStr(&rarityName))
                        {
                            int grade = ScanInt(&ok);
                            if (!ok)
                            {
                                throw static_cast<unsigned int>(0xc09);
                            }
                            int rarity = getItemRarity(rarityName.c_str());
                            if (rarity == 6)
                            {
                                throw static_cast<unsigned int>(0xc0c);
                            }
                            UPG_TBL_I32(table, rarity + 0x94) = grade;
                        }
                    }
                }
            }
            else if (token == "[destruction result]")
            {
                for (int t = 10; t <= 0x15; ++t)
                {
                    UPG_TBL_F32(table, t + 0x10b) = ScanFloat(&ok);
                    if (!ok)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                            0xc26, "importUpgradeTable - invalid type table");
                    }
                }
                int destroyIdx = ScanInt(&ok);
                if (!ok)
                {
                    return 1;                           // ORIG jmp EOF 成功出口
                }
                table->m_destruction.m_field58 = destroyIdx;
                for (;;)
                {
                    STResultItemCnt cnt;
                    cnt.m_a = static_cast<unsigned short>(ScanInt(&ok));
                    if (!ok)
                    {
                        break;
                    }
                    cnt.m_b = static_cast<unsigned short>(ScanInt(&ok));
                    if (!ok)
                    {
                        break;
                    }
                    cnt.m_c = static_cast<unsigned short>(ScanInt(&ok));
                    if (!ok)
                    {
                        break;
                    }
                    table->m_destruction.m_vec.push_back(cnt);
                }
            }
            else if (token == "[cost weight by upgrade level]")
            {
                for (;;)
                {
                    // ORIG 0x08901e81 起：每轮循环头先 `local_d9 = false` 复位 ok，
                    // 数值走 **ScanInt(bool*)/ScanFloat(bool*) 的返回值通道**
                    // （Ghidra `local_e0 = (float)ScanInt(&local_d9)`、
                    // `lVar8 = (longdouble)ScanFloat(&local_d9)`），ok 仅作
                    // 「!= true → 退出/返回 0」判定。此前写成 ScanInt(int*) 的
                    // 出参通道（P170：mangled 参数类型须 nm 实查），使本函数
                    // 引用了 ORIG 未调用的 _Z7ScanIntPi。
                    ok = false;
                    int level = ScanInt(&ok);
                    if (ok != true)
                    {
                        break;
                    }
                    float weight = ScanFloat(&ok);
                    if (ok != true)
                    {
                        return 0;
                    }
                    std::pair<std::map<int, float>::iterator, bool> ins =
                        table->m_floatMap.insert(std::make_pair(level, weight));
                    if (!ins.second)
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptEtcParameter.cpp", __FUNCTION__,
                            0xc54,
                            "[cost weight by upgrade level] key is duplicated]");
                        return 0;
                    }
                }
            }
            else if (token == "[upgrade machine frame base time]")
            {
                if (!ScanInt(&table->m_zero4b4[0]))
                {
                    return 0;
                }
                if (!ScanInt(&table->m_zero4b4[1]))
                {
                    return 0;
                }
                if (!ScanInt(&table->m_zero4b4[2]))
                {
                    return 0;
                }
                if (!ScanInt(&table->m_zero4b4[3]))
                {
                    return 0;
                }
            }
            else if (token == "[upgrade machine frame rate]")
            {
                for (;;)
                {
                    int key;
                    if (!ScanInt(&key))
                    {
                        break;
                    }
                    upgrade_table_t::MachineFrameInfo info;
                    if (!ScanFloat(&info.m_f00))
                    {
                        return 0;
                    }
                    if (!ScanFloat(&info.m_f04))
                    {
                        return 0;
                    }
                    if (!ScanFloat(&info.m_f08))
                    {
                        return 0;
                    }
                    if (!ScanFloat(&info.m_f0c))
                    {
                        return 0;
                    }
                    if (!ScanInt(&info.m_i10))
                    {
                        return 0;
                    }
                    // ORIG 0x08901e35：直接就地构造 value_type
                    // （_ZNSt4pairIKiN15upgrade_table_t16MachineFrameInfoEEC1IRiRS2_EEOT_OT0_
                    //  = pair<const int,MachineFrameInfo>::pair<int&,MachineFrameInfo&>）
                    // 后 _Hashtable::insert(value_type const&)，无 make_pair 转换临时。
                    // （上面 m_floatMap 那处 ORIG 确为 make_pair+转换，保持不动。）
                    table->m_machineFrames.insert(
                        std::unordered_map<int, upgrade_table_t::MachineFrameInfo>::value_type(
                            key, info));
                }
            }
        }
    }
    catch (unsigned int errLine)
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0xc14, "Error: (%s):%d",
                              "importUpgradeTable", static_cast<int>(errLine));
        return 0;
    }
