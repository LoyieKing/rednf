bool exportDungeonScript(STDungeonScript* script, const char* path)
{
    // b28d-solo-STDungeonScript 判词（1313 vs 1314 insn，p167 CLEAR 176=176）：
    // ①p167 差集修 7 处：3 个 pair 循环的 `it->first/second` → `(*it).first/second`
    //   （ORIG 发 `_ZNK…deEv` 非 `ptEv`，各 2 处共 6 个 operator-> → 0）；
    //   m_vec14 循环迭代器改「先默认构造后赋值」（ORIG 0x088f13df 独立 `…C1Ev`，
    //   P50 复活/P314）——p167 176=176 CLEAR。
    // ②[required item] 第三实参 `m_enableFlag != 0` → 三目 `? 1 : 0`：复现 ORIG
    //   0x088f0c3x 的 `test/setne→mov $1/mov $0` 分支物化（4.4 对 `!=0` 恒折 movzbl，
    //   gap 5→1）。
    // ③残余 DIFF = 槽号平移封顶（N82/P112）：file -0x64(ORIG) vs -0x6c、local 对象
    //   -0x5bc vs -0x5c8（帧同为 0x60c）；4.4 槽按「首次使用序」分配、4.1 按声明序，
    //   声明序不可翻（P112/N98）。另有 1 条 movl 残差。
    // 【验收 §6.2】超大函数（1314 insn）三件套：p167 CLEAR + gap 1 + 槽位集/帧全等。
    // ORIG 0x088f0a98（0x160c）——完整导出。按反汇编/Ghidra C 逐字段转写：
    //   fopen("wb") → STDungeonScript local（提供 m_a4 默认比较值）→ 各分区
    //   fprintf/fwrite 转储 → maze 循环（greed/map/boss/layered/start/boss）
    //   → spawn 系列 → 收尾 fclose + convertBinaryToText。
    // 【依赖】getScriptFileFullPath/convertBinaryToText/g_gridSymbols 见行内 extern。
    static char saveFileName[0x100];   // ORIG _ZZ19exportDungeonScriptP15STDungeonScriptPKcE12saveFileName
    getScriptFileFullPath(saveFileName, 0x100, g_szDungeonScriptBaseDirectory, path);

    FILE* file = fopen(saveFileName, "wb");
    if (!file)
        return 0;

    STDungeonScript local;   // ORIG 在栈上构造，仅用于 [coin limit] 与默认值比较

    fprintf(file, "[name] `%s`\n", script->m_str1c.c_str());
    fprintf(file, "[explain]\n`%s`\n", script->m_str20.c_str());
    fprintf(file, "[entering title] `%s`\n", script->m_str24.c_str());
    fprintf(file, "[cutscene image] `%s` %d\n", script->m_str28.c_str(), script->m_2c);
    fprintf(file, "[minimap image] `%s`\n", script->m_str30.c_str());
    fprintf(file, "[worldmap info]\n%d %d\n%d %d\n`%s`\n\n",
            script->m_5c[0], script->m_5c[1], script->m_5c[2], script->m_5c[3],
            script->m_str74.c_str());
    fprintf(file, "[minimum required level] %d\n", script->m_78);
    if (script->m_requiredItem7c.m_itemIdx != -1)
    {
        fprintf(file, "[required item] %d %d %d\n",
                script->m_requiredItem7c.m_itemIdx,
                script->m_requiredItem7c.m_itemCount,
                script->m_requiredItem7c.m_enableFlag ? 1 : 0);
    }
    if (script->m_a4 != local.m_a4)
        fprintf(file, "[coin limit] %d", script->m_a4);
    fprintf(file, "[basis level] %d\n", script->m_ac);
    fprintf(file, "[experience increasing point] %.3f\n", (double)script->m_b0);
    fprintf(file, "[champion] %d %d %d %d %d\n",
            script->m_b4[0], script->m_b4[1], script->m_b4[2], script->m_b4[3],
            script->m_b4[4]);
    // [E06 2026-09-10 真 bug 修正] ORIG 0x088f0d46/0x088f0d84/0x088f0e03 均 `+0xc8`
    //（`_ZNKSt6vectorISt4pairIiiESaIS1_EE5emptyEv` 实证）⇒ [item] 段导出的成员是
    // m_vecc8（tag "[item]" 的宿主，见 import case 0x1a6），非 m_vec98
    //（后者属 tag "[clear reward item]"，ORIG export 不导出）。
    if (!script->m_vecc8.empty())
    {
        fwrite("[item]\n", 1, 7, file);
        for (std::vector<std::pair<int, int> >::iterator it = script->m_vecc8.begin();
             it != script->m_vecc8.end(); it++)
            fprintf(file, "%d %d\n", (*it).first, (*it).second);   // P167：ORIG 发 deEv 非 ptEv
        fwrite("[/item]\n", 1, 8, file);
    }

    // [pathgate object]：+0xf8 POD 区 10 个具名 int
    fprintf(file, "[pathgate object]\n%d %d %d\n%d %d %d\n%d %d\n%d %d\n",
            script->m_pathgate.m_cells[0][0], script->m_pathgate.m_cells[0][1],
            script->m_pathgate.m_cells[0][2], script->m_pathgate.m_cells[1][0],
            script->m_pathgate.m_cells[1][1], script->m_pathgate.m_cells[1][2],
            script->m_pathgate.m_cells[2][0], script->m_pathgate.m_cells[2][1],
            script->m_pathgate.m_cells[3][0], script->m_pathgate.m_cells[3][1]);

    // ---- maze 循环 ----
    for (unsigned int mi = 0; mi < script->m_mazeList170.size(); ++mi)
    {
        MazeScript& maze = script->m_mazeList170[mi];
        fwrite("\n[maze info]\n", 1, 13, file);
        fprintf(file, "[size] %d %d\n", maze.m_width, maze.m_height);
        std::string greed;
        // ORIG 0x088f0fa4/0x088f0fab：`movl $0x0,-0x58` 连发两条 ⇒ 原码是
        // 「函数级/块级声明并初始化 + for 初始化」两段式（P191 配套），且 y 槽
        // 在后续 [map specification] 循环复用（ORIG 0x088f1065 同 -0x58）。
        int y = 0;
        for (y = 0; y < maze.m_height; ++y)
        {
            for (int x = 0; x < maze.m_width; ++x)
            {
                // ORIG 0x088f0fd6-0x088f0fd8：call getGrid 后直接
                // `mov (%eax),%eax; mov g_gridSymbols(,%eax,4),%eax`，无具名
                // GridScript* 缓存（P183/P329 去缓存）。第二段循环 ORIG
                // 0x088f1096 有 `mov %eax,-0x4c` ⇒ 那里保留具名缓存。
                greed += g_gridSymbols[maze.getGrid(x, y)->m_gridValue];
            }
            if (maze.m_height - 1 != y)
                greed += "\n ";
        }
        fprintf(file, "[greed]\n`%s`\n", greed.c_str());

        for (y = 0; y < maze.m_height; ++y)
        {
            for (int x = 0; x < maze.m_width; ++x)
            {
                GridScript* grid = maze.getGrid(x, y);
                if (!grid->m_mapCandidates.empty())
                {
                    fprintf(file, "[map specification]\n%d %d\n", x, y);
                    for (std::vector<int>::iterator it = grid->m_mapCandidates.begin();
                         it != grid->m_mapCandidates.end(); it++)
                        fprintf(file, "%d\n", *it);
                    fwrite("[/map specification]\n", 1, 21, file);
                }
                if (!grid->m_secondCandidates.empty())
                {
                    fprintf(file, "[boss map specification]\n%d %d\n", x, y);
                    for (std::vector<int>::iterator it = grid->m_secondCandidates.begin();
                         it != grid->m_secondCandidates.end(); it++)
                        fprintf(file, "%d\n", *it);
                    fwrite("[/boss map specification]\n", 1, 26, file);
                }
                if (!grid->m_layeredMapIndexes.empty())
                {
                    fwrite("[layered map specification]\n", 1, 28, file);
                    for (std::vector<int>::iterator it = grid->m_layeredMapIndexes.begin();
                         it != grid->m_layeredMapIndexes.end(); it++)
                        fprintf(file, "%d\n", *it);
                    fwrite("[/layered map specification]\n", 1, 29, file);
                }
            }
        }

        fwrite("[start map]\n", 1, 12, file);
        // ORIG 0x088f13df：m_vec14 循环的迭代器**先默认构造后赋值**
        // （独立 `iterator::C1Ev` 调用点；P50 复活/P314）。
        std::vector<std::pair<int, int> >::iterator it;
        for (it = maze.m_vec14.begin();
             it != maze.m_vec14.end(); it++)
            fprintf(file, "%d %d\n", (*it).first, (*it).second);
        fwrite("[/start map]\n", 1, 13, file);
        fwrite("[boss map]\n", 1, 11, file);
        // ORIG 0x088f14ff/0x088f1505：begin() 经独立 sret 临时 -0x5cc 再
        // `mov -0x5cc,%eax; mov %eax,-0xa0` 拷入具名 it（4.1 不做 copy-elision，
        // N86）。`*&f()`（P316）与显式转换临时两形实测均被 4.4 折叠 ⇒ 封 N86。
        // [E06 2026-09-10 复测] P50 形：复用上段先声明后赋值的 `it`（无 for 内声明）。
        for (it = maze.m_vec20.begin();
             it != maze.m_vec20.end(); it++)
            fprintf(file, "%d %d\n", (*it).first, (*it).second);
        fwrite("[/boss map]\n", 1, 12, file);
    }

    if (script->m_warRoom18c.m_10 != -1)
        fprintf(file, "[warroom map index] %d\n", script->m_warRoom18c.m_10);

    // [monster type spawn prob]
    fwrite("[monster type spawn prob]", 1, 25, file);
    for (unsigned int i = 0; i < script->m_warRoom18c.m_spawnProb.size(); ++i)
    {
        for (int j = 0; j < 4; ++j)
            fprintf(file, "%d ", script->m_warRoom18c.m_spawnProb[i].m_probability[j]);
        fputc(10, file);
    }
    fwrite("[monster type spawn cost]", 1, 25, file);
    for (int j = 0; j < 4; ++j)
        fprintf(file, "%d", script->m_warRoom18c.m_c4[j]);
    fputc(10, file);

    fprintf(file, "[player kc] %d", script->m_warRoom18c.m_d4);

    // ---- spawn common monster index ----
    if (script->m_warRoom18c.m_vec14[0].size() != 0)
    {
        fwrite("[spawn common monster index]\n", 1, 29, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec14[0].size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec14[0][i]);
        fwrite("[/spawn common monster index]\n", 1, 30, file);
    }
    // ---- spawn common champion index ----
    if (script->m_warRoom18c.m_vec14[1].size() != 0)
    {
        fwrite("[spawn common champion index]\n", 1, 30, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec14[1].size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec14[1][i]);
        fwrite("[/spawn common champion index]\n", 1, 31, file);
    }
    // ---- spawn super champion index ----
    if (script->m_warRoom18c.m_vec14[2].size() != 0)
    {
        fwrite("[spawn super champion index]\n", 1, 29, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec14[2].size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec14[2][i]);
        fwrite("[/spawn super champion index]\n", 1, 30, file);
    }
    // ---- spawn boss index ----
    if (script->m_warRoom18c.m_vec14[3].size() != 0)
    {
        fwrite("[spawn boss index]\n", 1, 19, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec14[3].size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec14[3][i]);
        fwrite("[/spawn boss index]\n", 1, 20, file);
    }

    fprintf(file, "[spawn step max] %d\n", script->m_warRoom18c.m_50);
    if (script->m_warRoom18c.m_vec44.size() != 0)
    {
        fwrite("[spawn step resource pool]\n", 1, 27, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec44.size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec44[i]);
        fwrite("[/spawn step resource pool]\n", 1, 28, file);
    }
    fprintf(file, "[kill count const] %d\n", (double)script->m_warRoom18c.m_ac);
    fprintf(file, "[join cost gold] %d\n", script->m_warRoom18c.m_54);
    fprintf(file, "[gold drop prob] %d\n", script->m_warRoom18c.m_58);
    fprintf(file, "[common monster item drop prob] %d\n", script->m_warRoom18c.m_5c[0]);
    fprintf(file, "[common champion item drop prob] %d\n", script->m_warRoom18c.m_5c[1]);
    fprintf(file, "[super champion item drop prob] %d\n", script->m_warRoom18c.m_5c[2]);
    fprintf(file, "[boss item drop prob] %d\n", script->m_warRoom18c.m_5c[3]);
    fprintf(file, "[common monster exp const] %d\n", script->m_warRoom18c.m_9c[0]);
    fprintf(file, "[common champion exp const] %d\n", script->m_warRoom18c.m_9c[1]);
    fprintf(file, "[super champion exp const] %d\n", script->m_warRoom18c.m_9c[2]);
    fprintf(file, "[boss exp const] %d\n", script->m_warRoom18c.m_9c[3]);

    // ---- 四条 item drop list ----
    if (script->m_warRoom18c.m_vec6c[0].size() != 0)
    {
        fwrite("[common monster item drop list]\n", 1, 32, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec6c[0].size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec6c[0][i]);
        fwrite("[/common monster item drop list]\n", 1, 33, file);
    }
    if (script->m_warRoom18c.m_vec6c[1].size() != 0)
    {
        fwrite("[common champion item drop list]\n", 1, 33, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec6c[1].size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec6c[1][i]);
        fwrite("[/common champion item drop list]\n", 1, 34, file);
    }
    if (script->m_warRoom18c.m_vec6c[2].size() != 0)
    {
        fwrite("[super champion item drop list]\n", 1, 32, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec6c[2].size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec6c[2][i]);
        fwrite("[/super champion item drop list]\n", 1, 33, file);
    }
    if (script->m_warRoom18c.m_vec6c[3].size() != 0)
    {
        fwrite("[boss item drop list]\n", 1, 22, file);
        for (unsigned int i = 0; i < script->m_warRoom18c.m_vec6c[3].size(); ++i)
            fprintf(file, "%d\n", script->m_warRoom18c.m_vec6c[3][i]);
        fwrite("[/boss item drop list]\n", 1, 23, file);
    }

    fprintf(file, "[battle spawn time] %d\n", script->m_warRoom18c.m_fc);
    if ((unsigned char)script->m_28c != 0)
        fprintf(file, "[ancient dungeon] %d", (unsigned int)(unsigned char)script->m_28c);
    if ((unsigned char)script->m_4e0 != 0)
        fprintf(file, "[risk dungeon] %d", (unsigned int)(unsigned char)script->m_4e0);
    if ((unsigned char)script->m_47c != 0)
        fprintf(file, "[SWAY EFFECT] %d %d \n", script->m_484, script->m_480);

    fclose(file);
    convertBinaryToText(saveFileName);
    return 1;
}