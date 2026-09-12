void PvP_MissionSystem::get_New_DailyMission(
    std::vector<std::pair<unsigned int, unsigned int> >& out)
{
    // ORIG 0x85e26dc 逐指令转写：三段——①dailyMissionIndices→map 查 mission
    // 推 {kind,idx}；②按 m_dailyMissionList 加权随机补 m_field30 个；③bitset
    // 校验 kind 范围/重复，越界或重复打印 cMyTrace 并清空 out。
    // [P2S1PvPQuest 2026-09-11 P167 语义轴] ORIG 全函数仅 **2 次**
    //   `_ZNSt6vectorISt4pairIjjESaIS1_EE5clearEv` 调用（0x85e2c2e / 0x85e2db2，
    //   即两个越界/重复分支内），**入口无 clear** ⇒ 旧文开头的 `out.clear();`
    //   是多余调用（EXTRA clearEv ×1）且改变语义（ORIG 是向传入 vector 追加）。
    //   [P2S1PvPQuest 2026-09-11 P167 负结论·已回滚] ORIG 0x85e26f5 入口确有
    //   `call _ZNSt17_Rb_tree_iteratorISt4pairIKiP8CMissionEEC1Ev`（默认构造，
    //   槽 -0xd0），但改"函数头默认构造 + 循环内赋值"实测 480→487（我方 +7 条），
    //   劣于循环内直接初始化（480→485）⇒ 不可达（4.4 对该形多发指令）。
    for (std::vector<unsigned int>::size_type i = 0;
         i < m_dailyMissionIndices.size(); ++i)
    {
        const unsigned int id = m_dailyMissionIndices[i];
        std::map<int, CMission*>::iterator it =
            m_missionMap.find(static_cast<int>(id));
        if (it == m_missionMap.end())
            continue;
        CMission* mission = it->second;
        if (mission == 0)
            continue;
        // [R26D] P167：ORIG 用 `_ZSt9make_pairIiiE`（int,int），旧文的多余
        // static_cast<unsigned int> 使其选 `make_pairIjjE`（MISSING i 版 ×2 /
        // EXTRA j 版 ×2）；push_back 处由 pair<unsigned,unsigned> 转换构造承接。
        out.push_back(std::make_pair(
            mission->GetMissionKind(),
            mission->GetMissionIndex()));
    }
    // [R26D · 轴 B 反向命中] ORIG 0x085e2ac0 段 `cmp -0x34(%ebp),%eax; seta %al`
    // （无符号序）⇒ 循环变量须为 unsigned；旧文 `int j` 使 4.4 产 `setg`
    // （有符号，/tmp/r5/axisB_points.tsv 记 seta→setg）。`j < m_field30`（int 成员）
    // 经同宽无指令转换即无符号比较，P196 前置已核 ORIG 为 seta ⇒ 允许改。
    // [R18scanCF 2026-09-13 容器循环不变量修复] ORIG 0x085e284d `call
    //   _ZNSt4listISt4pairIjjESaIS1_EEC1ERKS3_` 与 0x085e2852-58 `mov 0x3c(%eax);
    //   mov %eax,-0x44(%ebp)`（weight=m_field3c）均在 j 循环**外**——回边
    //   0x085e2ac8→0x085e2867 的循环体起点 0x085e2867 之后才进入循环体；旧文把
    //   二者写在循环体内（我方 .o 0x88510a8/0x88510ad 落在回边 0x8851343→0x8851095
    //   的循环体内）⇒ 我方每轮重建 tmp、重置 weight，而 ORIG 的 tmp.erase(li) 与
    //   weight -= li->first 跨轮持久 ⇒ 加权选取/去重序列分叉。按 ORIG 移出循环。
    std::list<std::pair<unsigned int, unsigned int> > tmp(m_dailyMissionList);
    int weight = m_field3c;
    for (unsigned int j = 0; j < (unsigned int)m_field30; ++j)
    {
        // [R4-13 2026-09-12 SEMANTIC-DIFF 修复] ORIG 0x085e2875/0x085e2878/0x085e287d
        //   `mov -0x44(%ebp),%ecx; mov $0x0,%edx; div %ecx` = **无符号**除法；
        //   旧文 `int r = randInt() % weight;` 产 `sar $0x1f,%edx; idivl`（有符号）
        //   ⇒ randInt() 高位/负值时取模结果不同。两操作数按 unsigned 取模后
        //   r 仍为 int（ORIG 0x085e28c7/0x085e28ca `cmp -0x2c(%ebp),%eax; jge` = 有符号比较，
        //   ⇒ 原码 r 为 int、仅除法为无符号）。同族先例：CInGameAdvertisementManager
        //   ::RandNum / CItemList::_FindItemRelateCreationRate 同法已修。
        int r = static_cast<unsigned int>(static_cast<CMTRand*>(m_trand)->randInt())
                % static_cast<unsigned int>(weight);
        int sum = 0;
        for (std::list<std::pair<unsigned int, unsigned int> >::iterator li =
                 tmp.begin(); li != tmp.end(); ++li)
        {
            sum += static_cast<int>(li->first);
            if (r < sum)
            {
                const unsigned int missionIdx = li->second;
                std::map<int, CMission*>::iterator mi =
                    m_missionMap.find(static_cast<int>(missionIdx));
                // [R26D] P167：ORIG 该点用 `==`（`_ZNK...eqERKS5_` ×1 差集）⇒
                // 写取反式而非 `!=`（ne 版是 EXTRA）。
                // [P2S1PvPQuest 2026-09-11 P167 负结论·已回滚] ORIG 0x85e2956 只调
                //   1 次 `ptEv`（`mov 0x4(%eax),%eax; mov %eax,-0x28(%ebp)` 后
                //   `cmpl $0x0,-0x28(%ebp); je +0x2fb`），但改"拆两级嵌套 if
                //   （m = mi->second; if (m != 0)）"实测 486→**476**（比 ORIG 480
                //   少 7 条，劣化）⇒ 回滚为 `&& mi->second != 0` 单链形
                //   （代价 = 多 1 次 ptEv call，实测优于拆分形）。
                // [R9a12 2026-09-12 SEMANTIC-DIFF 修复] ORIG 0x085e2947 `test %al,%al;
                //   jne 0x085e2a1e`：mi==end 时跳到 0x085e2a1e = `nop; ++li`
                //   （内层 list 循环继续），**不减 weight、不 tmp.erase(li)、不 break**；
                //   旧文把 mi==end 与 mi->second==0 合并（`!(mi==end) && mi->second!=0`
                //   的 false 边统一落到 0x085e29db 的 erase+break）⇒ mi==end 时误执行
                //   erase+break。两侧调用多重集 73=73 完全相同、p167 CLEAR ⇒ 只能靠
                //   逐 jcc 守卫存在性对照（PROTOCOL_R9 §14 #33）发现（#33 盲区实证）。
                if (mi == m_missionMap.end())
                    continue;
                // [R22a13 2026-09-13 §27.4 求值次数轴] ORIG 0x85e2956 为**单次**求值：
                //   `lea -0xd0; call ptEv; mov 0x4(%eax),%eax; mov %eax,-0x28(%ebp);
                //    cmpl $0x0,-0x28(%ebp); je +0x2fb`（一次 ptEv + 局部缓存后再判空）。
                //   旧形 `if (mi->second != 0) { CMission* m = mi->second; … }` 在同一轮内
                //   发 **2 次** ptEv（我方 .o 0x8854f1f 判空 + 0x8854f3d 取值，ORIG 仅
                //   0x85e2956 一处）⇒ 改为 ORIG 单次求值形（消 1 次 ptEv，仍用 arrow 形，
                //   ORIG 0x85e2792/0x85e2956 均为 `ptEv`、非 `deEv`）。
                CMission* m = mi->second;
                if (m != 0)
                {
                    out.push_back(std::make_pair(
                        m->GetMissionKind(),
                        m->GetMissionIndex()));
                }
                weight -= static_cast<int>(li->first);
                tmp.erase(li);
                break;
            }
        }
        if (weight <= 0)
        {
            // [P2S1PvPQuest 2026-09-11] P139：ORIG 该处引用 prett 符号
            //   `_ZZN17PvP_MissionSystem20get_New_DailyMissionERSt6vectorISt4pairIjjESaIS2_EEE19__PRETTY_FUNCTION__`
            //   @0x8cc3b60（size 0x9b，pstr.py 实测串 = 含完整 STL 实参的签名），
            //   旧文手写短串（43B）⇒ 缺 prett 符号。改 `__PRETTY_FUNCTION__` 发射同串。
            // [P2S1PvPQuest 2026-09-11] 格式串真缺口修复：ORIG 0x85e2a9b 实参
            //   `$0x8cc2b0c` = "[@missionSystem] InvalidMaxDice %d"（17B），旧文缺
            //   `[@missionSystem] ` 前缀（ORIG 0x8cc2b1d 的裸串是**别的引用者**的）。
            cMyTrace(__PRETTY_FUNCTION__, 0x123, 5)
                ("[@missionSystem] InvalidMaxDice %d", weight);
        }
    }
    std::bitset<32> dailyKind(0);
    for (std::vector<std::pair<unsigned int, unsigned int> >::size_type k = 0;
         k < out.size(); ++k)
    {
        const unsigned int kind = out[k].first;
        if (kind == 0)
            continue;
        if (kind > 0x20)
        {
            std::string s;
            for (std::vector<std::pair<unsigned int, unsigned int> >::size_type
                     m = 0; m < out.size(); ++m)
            {
                char buf[0x110];
                sprintf(buf, "<K:%d, I:%d>", out[m].first, out[m].second);
                s += buf;
            }
            cMyTrace(__PRETTY_FUNCTION__, 0x139, 5)
                ("[@missionSystem] OverKind: <Invalid mission kind : %d> "
                 "<mission index : %d> <Created DailyMission Size : %d> "
                 "<List : %s >",
                 kind, out[k].second, out.size(), s.c_str());
            out.clear();
        }
        else if (dailyKind[kind])
        {
            std::string s;
            for (std::vector<std::pair<unsigned int, unsigned int> >::size_type
                     m = 0; m < out.size(); ++m)
            {
                char buf[0x110];
                sprintf(buf, "<K:%d, I:%d>", out[m].first, out[m].second);
                s += buf;
            }
            cMyTrace(__PRETTY_FUNCTION__, 0x14d, 5)
                ("[@missionSystem] OverlappedKind: <Invalid mission kind : %d> "
                 "<mission index : %d> <Created DailyMission Size : %d> "
                 "<List : %s >",
                 kind, out[k].second, out.size(), s.c_str());
            out.clear();
        }
        else
        {
            dailyKind[kind] = true;
        }
    }
}