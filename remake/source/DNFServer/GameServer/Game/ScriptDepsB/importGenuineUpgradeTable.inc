// ============================================================================
// importGenuineUpgradeTable —— ORIG 0x08912336（../RDARScriptEtcParameter.cpp）
// 报告 _lt_global_gt_/importGenuineUpgradeTable.md。节分派：
//   [table]                    m_infos 清空后逐行 {int,int,int,level/10000f,float}
//                              以自增键插入 +0x158；
//   [separate upgrade max]     ScanInt(+0x004)，失败 logFormat(0x2f9e)；
//   [level]                    ScanInt(+0x000)，失败 logFormat(0x2fa5)；
//   [item weights by grade]    {key,int,int} 插入 +0x140；
//   [item weights by rarity]   6×float → +0x008 区（失败 logFormat(0x2fbe) 续扫）；
//   [stat weights by rarity]   6×float → +0x020 区（logFormat(0x2fc9)）；
//   [weapon weights]           11×6 float → +0x038 区（logFormat(0x2fd5)）；
//   [separate upgrade effect]  行循环 {r,g,b}→RGBA 打包入 +0x04，再
//                              {+0xc,+8,+0} 三 int，自增键插入 +0x170。
// loadRDARScriptFile 失败→false；其余路径均 true。
// ============================================================================
// [b28d-solo-ScriptDepsB NEAR(仅 RGBA 寄存器角色) | ORIG 0x08912336]
//  修法：补 ORIG 两枚死 string（-0xb0/-0xb4，P171 照抄）⇒ ctor/dtor 符号族归位；
//  两处 `make_pair(key++, X)` 后置 ++（右值第一参 ⇒ make_pair<int,T&>，P315）；
//  循环上界 `<= 5`/`<= 10` 复现 4.1 `setle;test;jne`（P51）。
//  ① 7 处 `continue` 改 `goto sectionHead`（标签在循环体首）：4.4 对 continue 恒发
//  共享 latch 块（`jmp <尾部>`，与 ORIG 直跳循环头 0x089123a2 形不符），goto 头标签
//  才逐条复现 ORIG 的 10 处 `jmp 0x089123a2`（含入口 3 个 `jmp;nop` 中继岛，
//  0x08912399/0x0891239c/0x0891239f）——探针 /tmp/sdb_probe/p5.o fGoto。
//  ② [separate upgrade effect] 尾 `continue`→`break`（真 bug：ORIG 0x0891285d
//  `je 0x089123a1` 后处理完即 0x089129c4 `jmp 退出块`，不再扫后续令牌）。
//  ③ RGBA 打包用 `(unsigned char)` 逐字节 cast（movzbl），`& 0xffu` 会退化成 and。
//  收口读数：raw 448=448、norm 444=444、p167 55/55 CLEAR；仅余 RGBA 首两操作数
//  寄存器角色（4.1 累加器留 %eax vs 4.4 留 %edx，P321-T5 寄存器角色 ⇒ N）。
//  [E06 2026-09-10 复测] 归一 diff 已收敛到单点：RGBA 打包段
//    ORIG `movzbl %dl,%edx; shl $0x8,%edx; or %edx,%eax`（累加器 %eax）
//    4.4  `movzbl %dl,%edx; shl $0x8,%edx; or %eax,%edx`（累加器 %edx）。
//  探针穷举 15 形（/tmp/e06_rgba/probe.cpp v1..v6 + probe2.cpp v7..v15：
//  原形/具名 unsigned/交换前两项/去 unsigned 提升/右结合/`&0xff` 掩码形/
//  三具名 int/uchar 局部/unsigned long 提升）**全部产 `or %eax,%edx`**；
//  `-fdump-tree-gimple` 证实 GIMPLE 树序未交换（`D = r | (g<<8)`），差异在
//  expand 阶段的可交换操作数 dst 选择 ⇒ 判 N86b（同一算术多 codegen 定论）+
//  P321-T5（寄存器角色不改类型/源码），**无源码旋钮**（探针路径见上）。
bool importGenuineUpgradeTable(const char* path,
                              GenuineDamageUpgradeTable* table)
{
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;
    std::string deadToken1;      // ORIG -0xb0（仅 ctor/dtor，P171 照抄）
    std::string deadToken2;      // ORIG -0xb4
    int level = 0;

    for (;;)
    {
    sectionHead:                 // ORIG 7 处 continue 均为 jmp 0x089123a2（直跳循环头）；
                                 // 4.4 对 continue 恒发共享 latch 块（jmp 尾部），
                                 // goto 头标签才复现直跳形（探针 /tmp/sdb_probe/p5.o fGoto）。
        if (!ScanType(token, true))
            break;

        if (token == "[table]")
        {
            int key = 1;
            table->m_infos.clear();
            for (;;)
            {
                GenuineDamageUpgradeInfo info;
                if (!ScanInt(&info.m_a))
                    break;
                if (!ScanInt(&info.m_b))
                    break;
                if (!ScanInt(&level))
                    break;
                info.m_levelRate =
                    static_cast<float>(level) / 10000.0f;
                if (!ScanFloat(&info.m_goldRate))
                    break;
                table->m_infos.insert(
                    std::make_pair(key++, info));   // 后置 ++ ⇒ make_pair<int,T&>
            }
            goto sectionHead;
        }

        if (token == "[separate upgrade max]")
        {
            if (!ScanInt(&table->m_maxLevel))
            {
                LogManager::logFormat(
                    0, "../RDARScriptEtcParameter.cpp",
                    __FUNCTION__, 0x2f9e,
                    "importGenuineUpgradeTable() - [separate_upgrade max] false");
            }
            goto sectionHead;
        }

        if (token == "[level]")
        {
            if (!ScanInt(&table->m_divisor))
            {
                LogManager::logFormat(
                    0, "../RDARScriptEtcParameter.cpp",
                    __FUNCTION__, 0x2fa5,
                    "importGenuineUpgradeTable() - [level] false");
            }
            goto sectionHead;
        }

        if (token == "[item weights by grade]")
        {
            for (;;)
            {
                int key = 0;
                GenuineDamageUpgradeMaterialInfo material;
                if (!ScanInt(&key))
                    break;
                if (!ScanInt(&material.m_materialItemIdx))
                    break;
                if (!ScanInt(&material.m_materialCount))
                    break;
                table->m_materials.insert(
                    std::make_pair(key, material));
            }
            goto sectionHead;
        }

        if (token == "[item weights by rarity]")
        {
            for (int i = 0; i <= 5; ++i)
            {
                if (!ScanFloat(&table->m_itemWeightsByRarity[i]))
                {
                    LogManager::logFormat(
                        0, "../RDARScriptEtcParameter.cpp",
                        __FUNCTION__, 0x2fbe,
                        "importGenuineUpgradeTable() - [item weights by rarity] false");
                }
            }
            goto sectionHead;
        }

        if (token == "[stat weights by rarity]")
        {
            for (int i = 0; i <= 5; ++i)
            {
                if (!ScanFloat(&table->m_statWeightsByRarity[i]))
                {
                    LogManager::logFormat(
                        0, "../RDARScriptEtcParameter.cpp",
                        __FUNCTION__, 0x2fc9,
                        "importGenuineUpgradeTable() - [stat weights by rarity] false");
                }
            }
            goto sectionHead;
        }

        if (token == "[weapon weights]")
        {
            for (int a = 0; a <= 10; ++a)
            {
                for (int b = 0; b <= 5; ++b)
                {
                    if (!ScanFloat(&table->m_weaponWeights[a][b]))
                    {
                        LogManager::logFormat(
                            0, "../RDARScriptEtcParameter.cpp",
                            __FUNCTION__, 0x2fd5,
                            "importGenuineUpgradeTable() - [weapon weights] false");
                    }
                }
            }
            goto sectionHead;
        }

        if (token == "[separate upgrade effect]")
        {
            int key = 1;
            for (;;)
            {
                OutLineEFFECTInfoTemp effect;
                int r = 0;
                int g = 0;
                int b = 0;
                if (!ScanInt(&r))
                    break;
                if (!ScanInt(&g))
                    break;
                if (!ScanInt(&b))
                    break;
                // ORIG 0x08912863..0x0891289a：(unsigned char)r | ((unsigned char)g
                //   << 8) | ((unsigned char)b << 16) | 0xff000000 → +0x04
                //   （movzbl 逐字节扩展；`& 0xffu` 形会退化成 and，见 /tmp/sdb_probe/p3.o）
                effect.m_b = static_cast<int>(
                    static_cast<unsigned>(static_cast<unsigned char>(r)) |
                    (static_cast<unsigned>(static_cast<unsigned char>(g)) << 8) |
                    (static_cast<unsigned>(static_cast<unsigned char>(b)) << 16) |
                    0xff000000u);
                if (!ScanInt(&effect.m_d))              // +0x0c
                    break;
                if (!ScanInt(&effect.m_c))              // +0x08
                    break;
                if (!ScanInt(&effect.m_a))              // +0x00
                    break;
                table->m_effects.insert(
                    std::make_pair(key++, effect));     // 后置 ++ ⇒ make_pair<int,T&>
            }
            // ORIG 0x0891285d `je <continue 岛>` + 内层循环尾 0x089129c4 `jmp 退出块`
            // ⇒ 本分支处理完即退出外层令牌循环（真 bug 修正：原写 continue 会继续
            //  扫描后续令牌，与 ORIG 行为不同）。
            break;
        }
    }

    return true;
}