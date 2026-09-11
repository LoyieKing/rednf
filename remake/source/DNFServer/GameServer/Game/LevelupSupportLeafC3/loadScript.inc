// ============================================================================
// LevelupSupportSciprt::loadScript —— 报告 loadScript.md @0x08ab514c。
// "Event/"+"tw_LevelupSupport.evt"；[step] → [level](ScanInt) → [reward]
// （循环 ScanTypeOrInt 取 item index、ScanInt 取数量，遇 TYPE 退出）→ [/step]
// （find 查重 → duplicate printf+return -1【此路径不 unloadRDARScriptFile，
// 忠实】/ new vector + swap 转移 + insert）。任一节内 ScanType 失败视为正常
// 结束：unloadRDARScriptFile 后返回 0；打开失败直接返回 -1。
// ============================================================================
// [b28d-N86] fresh 复验 210/213：残差 = std::string 局部（-0x44 vs -0x4c）+
// vector<ItemInfo> 构造/析构/swap 的 libstdc++ 外联槽整体平移 8B（P112/N86），
// 解析分支与全部调用点一一对应 ⇒ 语义等价、byte 层封顶。
int LevelupSupportSciprt::loadScript()
{
    if (!loadRDARScriptFile("Event/", "tw_LevelupSupport.evt"))
        return -1;

    std::string token;               // ORIG -0x44
    bool isType = false;             // ORIG -0x45（ScanTypeOrInt 出参）
    bool isInt = false;              // ORIG -0x46
    int curLevel = 0;                // ORIG -0x4c 当前 [level]
    std::vector<ItemInfo> rewards;   // ORIG -0x58 当前 [step] 奖励暂存

    // [M2S1] ORIG 条件在循环**顶部**且带 `xor $1`（0x08ab51b3..0x08ab51bd：
    // call; xor $1; test %al; jne +0x28a）⇒ 原码是 `for (;;) { if (!ScanType(...)) break; }`；
    // 旧 `while (ScanType(...))` 在 4.4 下条件挪到尾部且无 xor（探针 /tmp/probe_brk.o g4 vs g5）。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[step]")
        {
            if (!ScanType(token, true))
                break;
            if (token == "[level]")
                curLevel = ScanInt((bool*)0);
            if (!ScanType(token, true))
                break;
            if (token == "[reward]")
            {
                for (;;)
                {
                    // TYPE 终止奖励序列（该 token 由 ScanTypeOrInt 写入 token 缓冲并消费——忠实）。
                    int itemIndex = ScanTypeOrInt(isType, token, isInt, true);
                    if (isType)
                        break;
                    int itemCount = ScanInt((bool*)0);
                    rewards.push_back(ItemInfo(itemIndex, itemCount));
                }
            }
            if (!ScanType(token, true))
                break;
            if (token == "[/step]")
            {
                if (m_stepItems.find(curLevel) != m_stepItems.end())
                {
                    printf("duplicate [level] %d", curLevel);
                    return -1;   // 忠实：重复 [level] 路径不调用 unloadRDARScriptFile
                }
                std::vector<ItemInfo>* levelItems = new std::vector<ItemInfo>();
                levelItems->swap(rewards);   // swap 后 rewards 复位空，供下一 [step] 复用
                // [M2S1] ORIG 0x08ab53b0/0x08ab5c22：只构造一次 value_type
                // （pair ctor IRiRS6_E → insert），无 make_pair 中间临时
                // + 转换 ctor（我方旧形多 1 call + 一个 8B 槽 /tmp/m2probe_pair.o f1 vs f2）。
                m_stepItems.insert(std::map<int, std::vector<ItemInfo>*>::value_type(curLevel, levelItems));
            }
        }
    }

    unloadRDARScriptFile();
    return 0;
}