// ============================================================================
// importOldEquipmentStatInfo —— ORIG 0x08a58174（../RDARScriptOldEquip.cpp）
// 报告 _lt_global_gt_/importOldEquipmentStatInfo.md。
// 逐条读取 itemIndex + STEquipmentStatInfo 全部 26 个 int 字段；任一缺失即
// logFormat 并返回 false；重复 index 打印 Redundant 后跳过插入；
// ScanInt 收尾失败（正常 EOF）返回 true。
// ============================================================================
bool importOldEquipmentStatInfo(const char* dir)
{
    if (!loadRDARScriptFile(dir, "equipment/OldEquipmentStatInfoList.dat"))
    {
        LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                              __FUNCTION__, 0x12,
                              // Q7S4: ORIG 0x08e23968 为 CP949 原文（영문 아님），字节直证：
                              // 31 e2 c1 b8 ... ；我方此前发明英文串（AE 归一化掩盖）。
                              "CRITICAL ERROR : \xb1\xe2\xc1\xb8 \xc0\xe5\xba\xf1 \xbd\xba\xc5\xc8 \xc1\xa4\xba\xb8 \xb7\xce\xb5\xe5 \xbd\xc7\xc6\xd0!");
        return false;
    }

    unsigned int itemIndex = 0;
    bool ok;                       // ORIG 无预清（ScanInt 首写）
    // ORIG 帧头 `movl $0,-0x20/-0x1c/-0x18/-0x14`：四个 dead int 局部
    // （仅初始化、全程不读）——P31/P232 复现 ORIG sub $0x194 帧（我们少 0x10）。
    int dead0 = 0;
    int dead1 = 0;
    int dead2 = 0;
    int dead3 = 0;

    // ORIG 0x08a581f4：cond 前置（`call ScanInt; mov %eax,itemIndex;
    // movzbl ok; xor $1; test; jne exit`）+ body 后置 + 尾部 `jmp` 回边。
eq_loop:
    if (!(itemIndex = ScanInt(&ok), ok))
        goto eq_end;
    {
        STEquipmentStatInfo info;
        info.m_hpMax = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x29,
                                  "Invalid Old Equipment Stat Info(missing hpMax)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_mpMax = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x30,
                                  "Invalid Old Equipment Stat Info(missing mpMax)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_phyAtt = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x37,
                                  "Invalid Old Equipment Stat Info(missing physical attack)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_phyDef = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x3d,
                                  "Invalid Old Equipment Stat Info(missing physical defense)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_magAtt = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x43,
                                  "Invalid Old Equipment Stat Info(missing magical attack)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_magDef = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x49,
                                  "Invalid Old Equipment Stat Info(missing magical defense)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_equipPhyAtt = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x50,
                                  "Invalid Old Equipment Stat Info(missing equipment physical attack)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_equipPhyDef = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x57,
                                  "Invalid Old Equipment Stat Info(missing equipment physical defense)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_equipMagAtt = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x5e,
                                  "Invalid Old Equipment Stat Info(missing magical attack)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_equipMagDef = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x65,
                                  "Invalid Old Equipment Stat Info(missing magical defense)! itemIndex = %d", itemIndex);
            return false;
        }
        // 元素耐性 4 项（+0x28..+0x34）：ORIG 以 base+(i+8)*4 寻址
        // （m_view 基址 +0x08、索引 8+i；P311 成员数组视图）。
        for (int i = 0; i < 4; ++i)
        {
            info.m_view[2][i] = ScanInt(&ok);
            if (!ok)
            {
                LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                      __FUNCTION__, 0x6e,
                                      "Invalid Old Equipment Stat Info(missing element tolerance %d)! itemIndex = %d", i, itemIndex);
                return false;
            }
        }
        info.m_refAll = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x76,
                                  "Invalid Old Equipment Stat Info(missing all element tolerance)! itemIndex = %d", itemIndex);
            return false;
        }
        // 状态异常耐性 17 项（+0x3c..+0x7c）：ORIG 以 base+(j+12)*4 寻址
        // （m_view2 基址 +0x0c、索引 12+j；j>3 越界照抄 ORIG = P171）。
        for (int j = 0; j < 0x11; ++j)
        {
            info.m_view2[3][j] = ScanInt(&ok);
            if (!ok)
            {
                LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                      __FUNCTION__, 0x7f,
                                      "Invalid Old Equipment Stat Info(missing active property %d)! itemIndex = %d", j, itemIndex);
                return false;
            }
        }
        info.m_refAllState = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x87,
                                  "Invalid Old Equipment Stat Info(missing all activeStatus tolerance)! itemIndex = %d", itemIndex);
            return false;
        }

        // b28d-55（P220/P222 + P167 差集实证）：ORIG 用**正极性** `find == end`
        // （差集：ORIG 有 _Rb_tree_iterator::eq 而无 ne）+ 插入用
        // value_type(itemIndex, info) 引用构造（pair C1IRjRS1_），make_pair 形是
        // 我们多出的调用（P192/P178）。
        if (g_equipmentStatInfoMap.find(itemIndex) == g_equipmentStatInfoMap.end())
        {
            g_equipmentStatInfoMap.insert(
                std::map<unsigned int, STEquipmentStatInfo>::value_type(itemIndex, info));
        }
        else
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x92,
                                  "Redundant Old Equipment Stat Info! itemIndex = %d", itemIndex);
        }
    }
    goto eq_loop;

eq_end:
    return true;
}