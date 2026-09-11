// ORIG T 0x088ffe75 _Z29importUniqueRewardForAllUsersPcPSt3mapIiSt6vectorIS1_I22STStackablePackageDataSaIS2_EESaIS4_EESt4lessIiESaISt4pairIKiS6_EEE。
// [b28d-solo-OnlyOrigBatch14；gap 2→1 本波收敛]
// loadRDARScriptFile 参数序已修为 ("", path)（真语义 bug，ORIG 0x088ffe8d
// 压参序实证）。本波两条真缺口：
//   (a) `[package selection]` 内层循环 ORIG 088fff63-69 为负极性
//       `int itemIdx; for(;;){ if(!ScanInt(&itemIdx)) break; … }`（有 xor、
//       无 for-init `movl $0`），原 `for(int itemIdx=0; ScanInt(&itemIdx); )`
//       产 for-init+尾条件正极性 jne（多 movl、少 xor）；
//   (b) group 块尾 ORIG 0890002d `jmp 08900044` → jobs dtor → 0890004f
//       `jmp 088ffeca`（循环头）= 原码 **continue**（语义等价：token=="[group]"
//       时 "[/package]" 恒 false）。
// 残余 gap 1 = ORIG 的正常路径 jobs dtor 块（08900044-4f）物理位于 EH 段之后
// 并由 `jmp 08900044` 进入，4.4 把该 dtor 内联在 continue 点（N104 EH 尾块
// 相对序）。p167 MISSING `_ZSteqIcSt11char_traitsIcESaIcEE…EPKS3_`
// （string==const char* 外链实例，4.4 libstdc++ 全内联，N89）。
// [b14 复核] 槽序修复：P212「4.4 块内先声明占**浅**槽（4.1 相反）」⇒ 把
// `int itemIdx;` 提到 `std::vector<STStackablePackageData> items;` 之前，
// 复现 ORIG items@-0x30（12B，深）/ itemIdx@-0x24（4B，浅）的槽对
// （改前我方 itemIdx@-0x30 / items@-0x2c 恰好互换）。修复后 174 vs 173，
// 唯一残差即上述 `jmp +0x1cf` 单源 EH 尾岛（P223 负例：单源 goto 的尾岛
// 4.4 恒 jump-thread ⇒ 不可造，封 N）。
// 控制流/全部插入点与 ORIG 逐段核对，语义等价。
bool importUniqueRewardForAllUsers(
    char* path,
    std::map<int, std::vector<std::vector<STStackablePackageData> > >* out)
{
    if (out == 0)
        return false;
    // [b28d-solo-OnlyOrigBatch14] 真语义 bug（同 importQuestPieceScript）：
    // ORIG 0x088ffe8d 压参序 = loadRDARScriptFile(dir="", path)。
    if (!loadRDARScriptFile("", path))
        return false;
    std::string token;
    std::string name;
    // [gi00 四套配平补漏] 字段串 key 差集实证 ORIG 在本函数引用第 3 个 tag
    // `[/package]`（0x08900054 压参 + 0x08900062 比较）：其结构是 do-while 尾条件
    // `bVar2 = (token == "[/package]"); } while (!bVar2);` ⇒ 命中闭合标签即跳
    // LAB_08900072（与 ScanType 失败共用的 `return 1` 出口）。原实现缺该终止条件，
    // 会越过 `[/package]` 继续吞掉其后所有 `[group]` 块 ⇒ 可观察行为差（out 多填）。
    for (;;)
    {
        if (!ScanType(token, true))
            goto unique_reward_ok;
        if (token == "[group]")
        {
            if (!ScanStr(&name))
                return false;
            int job = getCharacterJob(name.c_str());
            std::vector<std::vector<STStackablePackageData> > jobs;
            ScanType(token, true);
            while (token == "[package selection]")
            {
                // [gap-2 修复] ORIG 088fff63-69：`lea -0x24; call ScanInt;
                // xor $1,%eax; test %al,%al; jne <跳出>`（负极性、无 for-init
                // `movl $0`）⇒ 原码是 `int itemIdx; for(;;){ if(!ScanInt(&itemIdx))
                // break; … }`，不是 `for(int itemIdx=0; ScanInt(&itemIdx); )`
                // （后者 4.4 产 for-init + 尾条件正极性 jne，少 xor、多 movl）。
                // [b14 槽序修复] P212：4.4 块内先声明占**浅**槽（4.1 相反）
                // ⇒ 把 itemIdx 声明提到 items 之前，使 items 占深槽 -0x30、
                // itemIdx 占浅槽 -0x24（与 ORIG 0x088fff58 段逐条一致）。
                int itemIdx;
                std::vector<STStackablePackageData> items;
                for (;;)
                {
                    if (!ScanInt(&itemIdx))
                    {
                        break;
                    }
                    STStackablePackageData p;
                    p.m_field0 = itemIdx;
                    p.m_field4 = ScanInt((bool*)0);
                    items.push_back(p);
                }
                jobs.push_back(items);
                ScanType(token, true);
            }
            (*out)[job] = jobs;
            // [gap-1 修复] ORIG 0890002d `jmp 08900044` → jobs dtor →
            // 0890004f `jmp 088ffeca`（循环头）⇒ 原码 group 块尾是 **continue**
            // （跳过 "[/package]" 检查直接回循环头；语义等价：token=="[group]"
            // 时 "[/package]" 恒 false）。4.4 对无 continue 的形让 dtor 块
            // 直落 "[/package]" 比较块，少这条 jmp。
            // [P1T1 本波复核] 残余唯一块 = 该 jmp：ORIG 0x890002d `jmp 0x8900044`
            // 跨过 0x890002f 起的 jobs 异常清理 pad（4.1 把 pad 排在语句落点后）；
            // 4.4 把该 pad 排到函数尾部 ⇒ 我方 continue 处直落
            // `lea -0x20(%ebp),%eax; call ~vector<vector>`（18c2，无前置 jmp）。
            // 删 continue 更劣（fall-through 进 "[/package]" 比较块）⇒ N82 pad 排布族封顶。
            continue;
        }
        if (token == "[/package]")
            break;
    }
unique_reward_ok:
    return true;
}
