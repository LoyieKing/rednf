bool CMultiMatchRoomList::GetWaitingRoomCombination(IMatch* me, int playCount,
                                                    std::vector<IMatch*>& out)
{
    // b28d-solo-MatchRoomList VERDICT: DIFF（447/446 均 424 vs 435）| 修法：
    // for+break 重构、totalPlayers 头声明、min 直读、doMatch 两独立守卫、
    // avgScore 前置、bestAvg/bestIdx 赋值序、count int 物化 | N82（槽布局）
    // + N86b（三维下标强度归约：4.4 恒 imul $0x8c/add 链 vs ORIG imul
    // $0x46/scaled；一维视图探针 g1b 实测净负已回退）
    // b28d-M2S3 复评（旧 N 判词逐条重试）：424 vs 434（gap +10）。已落地：
    // ①语义轴：`avgScore[i] == bestAvg` → `avgScore[i] < bestAvg`（ORIG
    //   0x8567486 `flds avgScore[i]; flds bestAvg; fucompp; fnstsw %ax;
    //   test $0x45,%ah; sete` 判 ST0>ST1 ⇒ bestAvg>avgScore[i] 才赋值 ⇒ 取最小；
    //   旧源 `==` 系 Ghidra 误读，属真语义 bug）；
    // ②P0：combIdx 去声明初值（-0x38 双写）、count 改 int（去 find 实参 int 临时）；
    // ③P329 求值序：两处 float 乘法左操作数换位（先 flds 数组 / mit->first）；
    // ④P222：内层 empty 检查改 continue 形（去 xor $1）；
    // ⑤P339：j/k/found/bestIdx/bestAvg/i 全头部声明（mnemonic 差块 31→17）；
    // ⑥P324：尾块序 `if (resultCount != 0) {…return 1;} return 0;`（17→15）。
    // 残余 15 块：4.4.6 强度归约 `add %eax,%eax` vs ORIG `lea (%eax,%eax,1),%edx`
    // （2i 生成差）×4 + 匿名临时槽阶（-0x68/-0x64、-0x50）。
    // 【A/B 直证】探针 /tmp/m2s3probe/Game/MatchRoomList.cpp：444 后端复现 ORIG
    // 的 lea 形，但整 TU 444 实测 blocks=39（劣于 446 的 17）⇒ 维持 446 路由；
    // 该残余判 N86b（后端 mul 展开差异，源码不可达）。
    // 与 ORIG 同名的函数内静态常量/组合表（数据导出自 0x8ca3200，
    // 0x8ca35d4 起为四个 1 字节常量）。
    static const unsigned char MAX_SAME_COUNT = 2;
    static const unsigned char MAX_WAITING_ROOM_COUNT = 7;
    static const unsigned char MAX_COMBINATION_COUNT = 10;
    static const unsigned char MAX_REMAIN_WAITING_ROOM = 7;
    static const unsigned char CombinationTable[MAX_REMAIN_WAITING_ROOM]
        [MAX_COMBINATION_COUNT][MAX_WAITING_ROOM_COUNT][MAX_SAME_COUNT] = {
        /* playCount=0（无效区，全 0） */
        { {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
        /* playCount=1：1 队 x 1 人 */
        { {1,1,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
        /* playCount=2：1x2 / 2x1 */
        { {1,2,0,0,0,0,0,0,0,0,0,0,0,0}, {2,1,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
        /* playCount=3：1x3 / 2x1+1x1 / 3x1 */
        { {1,3,0,0,0,0,0,0,0,0,0,0,0,0}, {2,1,1,1,0,0,0,0,0,0,0,0,0,0},
          {3,1,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
        /* playCount=4：1x4 / 2x1+1x2 / 2x1+2x1 / 3x1+1x1 / 4x1 */
        { {1,4,0,0,0,0,0,0,0,0,0,0,0,0}, {2,1,1,2,0,0,0,0,0,0,0,0,0,0},
          {2,1,2,1,0,0,0,0,0,0,0,0,0,0}, {3,1,1,1,0,0,0,0,0,0,0,0,0,0},
          {4,1,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
        /* playCount=5：1x5 / 2x1+1x3 / 2x2+1x1 / 3x1+2x1 / 3x1+1x2 / 4x1+1x1 */
        { {1,5,0,0,0,0,0,0,0,0,0,0,0,0}, {2,1,1,3,0,0,0,0,0,0,0,0,0,0},
          {2,2,1,1,0,0,0,0,0,0,0,0,0,0}, {3,1,2,1,0,0,0,0,0,0,0,0,0,0},
          {3,1,1,2,0,0,0,0,0,0,0,0,0,0}, {4,1,1,1,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
        /* playCount=6：1x6 / 2x1+1x4 / 2x2+1x2 / 2x3 / 3x1+1x3 /
           3x1+2x1+1x1 / 4x1+1x2 / 4x1+2x1 / 5x1+1x1 */
        { {1,6,0,0,0,0,0,0,0,0,0,0,0,0}, {2,1,1,4,0,0,0,0,0,0,0,0,0,0},
          {2,2,1,2,0,0,0,0,0,0,0,0,0,0}, {2,3,0,0,0,0,0,0,0,0,0,0,0,0},
          {3,1,1,3,0,0,0,0,0,0,0,0,0,0}, {3,1,2,1,1,1,0,0,0,0,0,0,0,0},
          {4,1,1,2,0,0,0,0,0,0,0,0,0,0}, {4,1,2,1,0,0,0,0,0,0,0,0,0,0},
          {5,1,1,1,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0,0,0,0,0,0,0} },
    };

    // [N1C·2026-09-11] ORIG find 的实参槽 = -0x50，紧贴 matched(-0x68..-0x51)
    // 之后 ⇒ count 与函数块变量同批分配（块内具名局部会抢 -0x68 把 matched
    // 挤到 -0x64，产生 9 处 `lea -0x64` vs `-0x68` 残差；探针 v4/v7 实证
    // 上移声明后该 9 处全消）。语义不变：每轮 j 都在使用前赋值。
    int count;
    int totalPlayers = 0;
    float avgScore[MAX_COMBINATION_COUNT] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                             0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    std::vector<IMatch*> combination[MAX_COMBINATION_COUNT];
    std::multimap<float, IMatch*> matched;
    int resultCount = 0;
    // b28d-S3（P0）：ORIG 0x85670e2 只写一次 -0x34；旧源声明处 `= 0` + for-init
    // 再写一遍 ⇒ 我方 -0x38 双写（+1 movl）。去声明初值，保留 for-init。
    int combIdx;
    // b28d-M2S3（P339）：ORIG 具名槽阶梯 = combIdx(-0x34) j(-0x30) k(-0x2c)
    // found(-0x25) bestIdx(-0x24) bestAvg(-0x20) i(-0x1c) ⇒ 全部函数头声明，
    // 初始化仍留在原语句位置（ORIG 0x856741d 起才写初值）。旧源把 j/k/i 声明
    // 在块内 ⇒ 槽落到 -0x24/-0x20（与 ORIG 差 0xc）。
    int j;
    int k;
    bool found;
    int bestIdx;
    float bestAvg;
    int i;

    // b28d-solo-MatchRoomList: for 循环 + 首行标志 break（ORIG 条件在底部、
    // 收尾块在循环外）；totalPlayers 函数头声明、每轮重置。
    // 注：一维视图 (ci*7+pc*70+j)*2 探针（/tmp/b28d_gwc_probe.cpp g1b）实测
    // 外层 j=0 被 4.4 常量折叠回 imul $0x8c、内层多 1 条中转 ⇒ 净负，保留
    // 三维下标（4.4 强度归约与 ORIG 不同形，判 N86b）。
    // [N1C·2026-09-11 · 负结论] 外循环首项检查（ORIG 0x085670ee..0x08567112）的
    // combIdx*14 归约形：ORIG=`lea (%eax,%eax,1),%edx; lea 0x0(,%edx,8),%eax;
    // sub %edx,%eax; imul $0x8c,%ecx,%edx; add %edx,%eax`，我方=`add %eax,%eax;
    // lea 0x0(,%eax,8),%edx; sub %eax,%edx; imul $0x8c,%ecx,%eax;
    // lea (%edx,%eax,1),%eax`（6 条，仅临时寄存器选择差，语义等价）。探针
    // t4/t5/t9/t12（'\0' / yoda / unsigned 下标 / int 下标）与 count 位置
    // t2/t3 均不改变该形 ⇒ 判 regalloc N 类（源码不可达）；件残差仅此 1 块。
    for (combIdx = 0; combIdx <= 9; ++combIdx) {
        if (CombinationTable[playCount][combIdx][0][0] == 0)
            break;
        totalPlayers = 0;
        for (j = 0; j < MAX_WAITING_ROOM_COUNT; ++j) {
            if (CombinationTable[playCount][combIdx][j][0] == 0)
                break;
            matched.clear();
            // ORIG：if 检查为表达式直读（test %al），find 侧再算一次并物化 int 槽
            // b28d-S3：ORIG 0x856717f 单槽 `mov %eax,-0x50` + `lea -0x50` 直传
            // find；旧源 `unsigned int` 触发 int 临时转换（多 2 条 mov）。
            count = (unsigned char)CombinationTable[playCount][combIdx][j][0];
            // ORIG 对 find 结果不做 end() 检查（GetPlayCountPerOneTeam=1 时
            // 组合表只使用 key=1 的列表），此处忠实还原。
            std::map<int, CWaitingRoomList>::iterator it = m_roomMap.find(count);
            // ORIG 两个独立守卫（xor$1/test/jne 与 size<min 各一 break 岛）
            if (!it->second.doMatch(me, matched))
                break;
            if (matched.size() <
                CombinationTable[playCount][combIdx][j][1])
                break;
            k = 0;
            for (std::multimap<float, IMatch*>::iterator mit = matched.begin();
                 mit != matched.end() &&
                 k < CombinationTable[playCount][combIdx][j][1];
                 ++mit, ++k) {
                // b28d-S3（P329 求值序）：ORIG 0x8567233 先 `mov -0x34,%ebx` +
                // `flds avgScore[combIdx]` 再 `fildl -0x3c` ⇒ 乘法左操作数 =
                // avgScore[combIdx]；第二项 ORIG 0x8567250 先 `flds mit->first`
                // 溢出 -0x128 再取 GetPlayerCount() ⇒ 左 = mit->first。
                // 两处均为 float 乘法，交换律 ⇒ 语义不变，仅求值序对齐。
                avgScore[combIdx] = (avgScore[combIdx] * totalPlayers +
                                     mit->first * mit->second->GetPlayerCount()) /
                                    (float)(mit->second->GetPlayerCount() + totalPlayers);
                totalPlayers += mit->second->GetPlayerCount();
                combination[combIdx].push_back(mit->second);
            }
        }
        if (!combination[combIdx].empty()) {
            if (totalPlayers == playCount)
                ++resultCount;
            else
                combination[combIdx].clear();
        }
    }
    // b28d-M2S3（P324 块序）：ORIG 0x8567413 `cmpl $0x0,-0x38; je <return-0 块
    // +0x137>` ⇒ 成功块直落、return 0 跳后。旧源 `if (resultCount == 0)
    // return 0;` 令 4.4 发 `jne <成功块>`（return-0 块直落），块序相反。
    if (resultCount != 0)
    {
        found = false;
        bestIdx = 0;
        bestAvg = 0.0f;
        for (i = 0; i < MAX_COMBINATION_COUNT; ++i) {
            // b28d-S3：ORIG 0x8567451 `call empty; test %al,%al; jne <循环尾>`
            // （无 `xor $1`）⇒ 源码为 continue 形（旧源 `if (!...)` 多 1 条 xor）。
            if (combination[i].empty())
                continue;
            if (!found) {
                bestIdx = i;
                bestAvg = avgScore[i];
                found = true;
            }
            // [语义轴修正·b28d-S3] 旧源 `avgScore[i] == bestAvg` 是错译：ORIG
            // 0x8567486 形 `flds avgScore[i]; flds bestAvg; fucompp; fnstsw %ax;
            // test $0x45,%ah; sete`（C3C2C0=000 ⇔ ST0>ST1 ⇔ bestAvg>avgScore[i]）
            // 且赋值块 bestAvg=avgScore[i] ⇒ 真语义 = 取最小 avgScore。
            if (avgScore[i] < bestAvg) {
                bestAvg = avgScore[i];
                bestIdx = i;
            }
        }
        out.insert(out.end(), combination[bestIdx].begin(),
                   combination[bestIdx].end());
        return 1;
    }
    return 0;
}