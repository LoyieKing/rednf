// ============================================================================
// loadScript —— ORIG T 0x88b1b36。"Etc/EmblemRenewal/" + "avatar_convert_jpn.etc"。
// 每条记录：TYPE 头 → grade=ScanInt、flag=(ScanInt==1) → 循环 ScanTypeOrInt 取
// 数值键 + 2×short（push GetInfo{key,idx,cnt}，累计 cnt 入 m_cumWeights）→
// 因 TYPE 退出时查重插入 m_convertMap（total=m_cumWeights.back()），因 EOF/非法
// 退出则整条丢弃。重复 grade：printf("%s duplicate grade:%d") 后返回 -2
// （ORIG 此路径不释放 cr——忠实保留）。
// 返回 0=成功 / -1=打开失败 / -2=重复 grade。
// ============================================================================
// [b28d-solo-AvatarConvertServer] NEAR（ORIG 0x088b1b36，190 vs 190，助记符序列全等，
// p167 CLEAR 25/25）。修法：P299/P318 value-init（删 ConvertResult 用户 ctor/dtor ⇒
// `new ConvertResult()` 前置零初始化，消原 operator new+memset+placement new 的 N83）+
// P164 循环头形（for(;;)+if(!ScanType) break，复现 4.1 入口直落条件头）+
// P0 去 key 具名局部（info.m_key 直收 ScanTypeOrInt 返回值）+ P220 正极性 find==end +
// field20 具名临时（复现 ORIG -0x1c 比较前落槽）。
// [LaneD nearA 2026-09-11 改善] 旧判「槽号整体浅 4（N82/P112）」真因查明：
// ORIG 函数头存在一个**未被引用的 4 字节局部**（位于最深槽，反汇编不可见），
// 它使 ORIG 的 token/isType/isInt 组整体比我方深 4。加死数组
// `char framePad4[4]; (void)framePad4;`（P31/P232）后该组逐值对齐
// （token -0x38 / isType -0x39 / isInt -0x3a，与 ORIG 全等），**ndiff 32→2**、
// 帧 0x5c 不变、TU 四数零回退（102/183/1/4）。
// 残余 1 处（ndiff=2）：块内 endIt 槽 ORIG -0x34 vs 我方 -0x50（`end()` sret 两点）。
// 已试并回滚：函数头声明 `iterator endIt;` + 块内赋值 ⇒ 4.4 发迭代器默认 ctor call
// （+1 call/帧 0x5c→0x6c/ndiff 28）劣化；ORIG 无该 call（P215：4.1 空默认 ctor 展开 0 条）
// ⇒ 该槽不可在 4.4 下复现，判 N95/N82 槽分配族，语义完成。
int AvatarConvert::loadScript()
{
    if (!loadRDARScriptFile("Etc/EmblemRenewal/", "avatar_convert_jpn.etc"))
        return -1;

    // [LaneD nearA 2026-09-11 已落地·改善] P31/P232：ORIG 本组（token/isType/isInt）
    // 槽位整体较我方深 4（-0x38/-0x39/-0x3a vs -0x34/-0x35/-0x36）= ORIG 函数头多一个
    // 未被引用的 4 字节局部（deepest，故反汇编不可见）。本死数组即该槽的等价复现：
    // 组槽逐值对齐、帧仍 0x5c、ndiff 32→2、TU 四数零回退。
    char framePad4[4];
    (void)framePad4;
    std::string token;
    bool isType = false;
    bool isInt = false;
    for (;;)
    {
        if (!ScanType(token, true))
            break;               // P164：条件落循环头，复现 4.1 入口直落条件头
        int totalWeight = 0;   // ORIG -0x40：每条记录重置
        // ORIG 0x088b1baf：`new ConvertResult()` 的 value-init 前置零初始化
        // （operator new(0x24) → 8×rep stos dword + 1B → call C1Ev）。
        ConvertResult* convertResult = new ConvertResult();
        convertResult->m_grade = ScanInt((bool*)0);
        int field20 = ScanInt((bool*)0);   // ORIG -0x1c：比较前落槽
        convertResult->m_field20 = (field20 == 1);
        for (;;)
        {
            GetInfo info;
            // P0：ORIG 无具名 key 局部——ScanTypeOrInt 返回值直落 info.m_key。
            info.m_key = ScanTypeOrInt(isType, token, isInt, true);
            if (isType)
                break;   // TYPE → 插入分支
            if (!isInt)
                break;   // EOF/非法 → 丢弃该记录
            info.m_count = static_cast<short>(ScanInt((bool*)0));
            info.m_weight = static_cast<short>(ScanInt((bool*)0));
            convertResult->m_infos.push_back(info);
            totalWeight += info.m_weight;
            convertResult->m_cumWeights.push_back(totalWeight);
        }
        if (isType)
        {
            std::map<int, ConvertResult*>::iterator endIt = m_convertMap.end();
            if (m_convertMap.find(convertResult->m_grade) == endIt)
            {
                convertResult->m_totalWeight = convertResult->m_cumWeights.back();
                m_convertMap.insert(
                    std::map<int, ConvertResult*>::value_type(convertResult->m_grade,
                                                              convertResult));
            }
            else
            {
                printf("%s duplicate grade:%d", token.c_str(), convertResult->m_grade);
                return -2;
            }
        }
    }
    unloadRDARScriptFile();
    return 0;
}