// ============================================================================
// importQuestShopScript — ORIG 0x0890fa3d（init.md +0x10d4，
// "Etc/questshop.etc" → m_questShop）。返回 0=成功，否则错误码：
//   0x2815 装载失败 / 0x2829 [quest status] 序号 ScanInt 失败 /
//   0x284f loadAppendageType==-1 / 0x286d [quest point] ScanInt 失败。
// "[initialization costs]" 三值写 m_initCost（失败按 ORIG 返回 0）。
// ============================================================================
// [b28d-solo-InitScriptProducers_7] verdict=DIFF [N82/P71] ORIG 0x0890fa3d
//  已修：status 提函数头（-0xcc）；[quest status]/[initialization costs] 改
//  if/else 链（p167 MISSING eq → == 正判）；map 插入改 make_pair
//  （p167 MISSING make_pair/pair C1 OS_ 变体，P61/P192）。
//  残余：p167 仅 EXTRA `stStatusLevelInfoD1Ev` ×2 —— P71 用户空析构跨版本
//  trivial 判定差（CDataManager.h 共享头，不改）；槽/临时分配 N82。
// [b28f-SweepDeps 复核] 主循环 `while (ScanType(token,true))`：ORIG 0x0890fa9d
//  为**头置条件**（`movl $0x1,0x4(%esp); call ScanType; xor $0x1,%eax; test;
//  jne exit`，入口 0x0890fa9a `jmp +2` 跳过对齐 nop），4.4 恒旋转为底部条件
//  （entry `jmp <尾条件>`）。P292/P283 形（for(;;)+`if(!ScanType) break;`）实测
//  **反劣**：our 370→372、gap −27→−29（diff 行 525→517 不改变 verdict）⇒ 已回退，
//  按 P164 环旋转（4.4 固有）封顶。同族 importPcBangEquipmentRendar 内层
//  items 循环已记「三形穷举不可同时保 xor$1 与头置」。
// [b28f-SweepDeps 线索] 另 2 处 EXTRA `stStatusLevelInfoD1Ev` 可试 P334 trivial
//  镜像视图（本 TU anonymous namespace 同字段序 POD 镜像 + 容器实参位
//  reinterpret_cast<const stStatusLevelInfo*>），未实测（超本波预算）。
int importQuestShopScript(const char* path, stQuestShop& out)
{
    if (!loadRDARScriptFile("", path))
    {
        return 0x2815;
    }

    // [b28d-solo-InitScriptProducers_7] ORIG 0x0890fa3d 声明序/槽：
    // token -0x80 / ok -0x81 / text -0x88 / status -0xcc（status 函数头 C1，
    // 每 [quest status] 记录调 clear；旧版块内声明致帧 0xb0 vs ORIG 0xe0）。
    std::string token;                         // ORIG -0x80
    bool ok = false;                           // ORIG -0x81
    std::string text;                          // ORIG -0x88
    stQuestShopStatus status;                  // ORIG -0xcc

    while (ScanType(token, true))
    {
        // ORIG 0x0890fac9：[quest status] 用 operator== 正判、else 落
        // [initialization costs]（p167：MISSING eq / EXTRA ne）；map 插入用
        // make_pair（p167：MISSING make_pair/pair C1<...>OS_ 变体，P61/P192）。
        if (token == "[quest status]")
        {
            // ---- 单个 [quest status] 记录 ----
            status.clear();
            const int idx = ScanInt(&ok);
            if (!ok)
            {
                return 0x2829;
            }
            if (out.m_field18 < idx)
            {
                out.m_field18 = idx;
            }

            bool closeRecord = false;
            while (!closeRecord)
            {
                if (!ScanType(token, true))
                {
                    closeRecord = true;         // ORIG：流结束也先入库再回主循环
                    break;
                }
                if (token == "[/quest status]")
                {
                    closeRecord = true;
                    break;
                }
                if (token == "[name]")
                {
                    ScanStr(&status.m_name);
                }
                else if (token == "[name2]")
                {
                    ScanStr(&status.m_name2);
                }
                else if (token == "[explain]")
                {
                    ScanStr(&status.m_explain);
                }
                else if (token == "[icon]")
                {
                    ScanStr(&status.m_icon);
                    status.m_field10 = ScanInt((bool*)NULL);
                }
                else if (token == "[quest status info]")
                {
                    if (!ScanStr(&text))
                    {
                        return 0x284f;
                    }
                    const int appendage = loadAppendageType(text);
                    if (appendage == -1)
                    {
                        return 0x284f;
                    }
                    if (!ScanStr(&text))
                    {
                        return 0x284f;
                    }
                    const int valueType = loadValueType(text);
                    (void)valueType;
                    const float value = ScanFloat((bool*)NULL);
                    (void)value;
                    // ORIG：out+0x28+appendage = (char)idx（appendage 槽位标记）
                    out.m_pad28[appendage] = static_cast<char>(idx);
                }
                else if (token == "[level info]")
                {
                    // [b56FIX · 缺 case 内循环 ⇒ 整个 [quest status] 记录被截断]
                    // ORIG 0890fcdf..0890fd50：[level info] 的 case 体是**循环**——
                    //   0890fcdf `call stStatusLevelInfoC1`（ctor 在环内，每轮重建）
                    //   0890fcf6 ScanInt(bool*)→info.m_second；!ok → `jmp 0x890fd52`
                    //   0890fd1a ScanInt(bool*)→info.m_first ；!ok → `jmp 0x890fd52`
                    //   0890fd4b `status.m_levelInfos.push_back(info)`
                    //   0890fd50 `jmp 0x890fcdf` ⇐ **回到环首再读一对**
                    //   0890fd52 `jmp 0x890fb2f`（内层 while(!closeRecord) 头）
                    // 我方旧形一个 case 只读**一对**就落回内层 while，于是剩余的数字被
                    // 内层 `ScanType` 一个个吞（ok 一旦为 false 不复位），撞到
                    // `[/level info]` 这个 TYPE 时被判为失败 ⇒ 内层循环提前结束 ⇒
                    // 同一个 [quest status] 记录里 `[quest point]` 之后的内容全部
                    // 被外层循环当成新记录解析，`[/quest status]` 再无匹配 ⇒ 外层
                    // `ScanType` 也返回 false ⇒ import 提前收尾。
                    // 实机 A/B 实证（Etc/questshop.etc，/tmp/ab_qt_{ours,orig}.js 对钩
                    // getToken/getType/ScanType）：两条 token 流在前 49 个词**完全一致**，
                    // 第 50 个词（TYPE `[/level info]`）我方带 `getType`（=被 ScanType 消费，
                    // 返回地址 @f8 内层头），ORIG **不带 getType**（=被失败的 ScanInt 消费）
                    // ⇒ 随后我方 END ret=0 只走了 53 个词，ORIG 继续走完 589 个词。
                    for (;;)
                    {
                        stStatusLevelInfo info;
                        info.m_second = static_cast<unsigned char>(ScanInt(&ok));
                        if (!ok)
                        {
                            break;
                        }
                        info.m_first = static_cast<unsigned char>(ScanInt(&ok));
                        if (!ok)
                        {
                            break;
                        }
                        status.m_levelInfos.push_back(info);
                    }
                }
                else if (token == "[quest point]")
                {
                    const int point = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0x286d;
                    }
                    status.m_questPoint = static_cast<unsigned char>(point);
                }
            }

            out.m_map0.insert(std::make_pair(idx, status));
        }
        else if (token == "[initialization costs]")
        {
            stQuestShoptInitCost cost;
            cost.m_cost0 = ScanInt(&ok);
            if (!ok) { return 0; }
            cost.m_cost1 = ScanInt(&ok);
            if (!ok) { return 0; }
            cost.m_cost2 = ScanInt(&ok);
            if (!ok) { return 0; }
            out.m_initCost.m_cost0 = cost.m_cost0;
            out.m_initCost.m_cost1 = cost.m_cost1;
            out.m_initCost.m_cost2 = cost.m_cost2;
        }
    }
    return 0;
}