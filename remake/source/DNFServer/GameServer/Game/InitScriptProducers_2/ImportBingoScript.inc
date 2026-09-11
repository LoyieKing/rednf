// ============================================================================
// ImportBingoScript —— ORIG T 0x089187b9（init.md:1071，"Event/BingoEvent.evt"
// → this+0xb568）。ORIG 所有路径恒返回 0（含加载失败），忠实保留。
// 段：[board]→m_map18、[quiz value]→m_map0、[reward]→m_map30、[use item]→m_vec48。
// ============================================================================
// [b28d-solo-InitScriptProducers_2] verdict=N82/N89 封顶（原 DIFF 311）ORIG
//   0x089187b9 orig/ours=337/338，P167 CLEAR（41/41）。本波修 p167 差集：
//   ①make_pair 全部去 const 左值（ORIG 3 实例 <int&,vector<int>&>/<int&,int&>/
//   <int&,pair<int,int>&>）；②补 `values.clear()`（ORIG 0x089188ad 显式调用）；
//   ③上限变量化 `int limit = 25`（ORIG -0x20 存 0x19，setl 形）；④P164 goto
//   主循环（0x08918800）；⑤count/ok 失败改尾岛 goto scan_done。残余：槽位/块
//   布局跨版本差。
// [P330 bool→int] 与 DataManagerInitDepends.h 声明同步（ORIG caller 4B store
// 无 movzbl ⇒ int）。ORIG 所有出口 ebx=0 ⇒ 两个 return 0。
// [sweep2-InitScriptProducers_2] 复核：fresh orig/ours=329/328 gap=1 ndiff=17，
//   ORIG-only={je:2,jmp:1} vs OURS-only={jne:2}（循环尾 je-exit/jmp-body 布局）；
//   p167 41/41 仅 C1↔C2 同址别名（P93）⇒ 无真缺口，N 封顶。
int ImportBingoScript(const char* path, stBingoScript& out)
{
    if (!loadRDARScriptFile("", path))
        return 0;

    std::string token;
    bool ok = false;

    // P164：纯 goto 循环复位头部条件块（ORIG 0x08918800）。
scan_top:
    if (!ScanType(token, true))
        goto scan_done;
    {
        if (token == "[board]")
        {
            const int count = ScanInt(&ok);
            if (!ok)
                goto scan_done;                          // ORIG 0x08918c96 尾部岛
            // ++i 必须留在 for 增量位：ORIG 0x089189d3 `addl $1,-0x24` 在
            // values 析构（0x089189ce D1）之后；写进循环体会被 4.4 提前到析构前。
            for (int i = 0; i < count; ++i)
            {
                // 非 const 左值 ⇒ ORIG make_pair<int&,vector<int>&>（p167）。
                int key = ScanInt(&ok);
                if (!ok)
                    break;                               // 直接回主循环，不插入

                std::vector<int> values;                 // 每行新建（ORIG ctor+clear）
                values.clear();                          // ORIG 0x089188ad 显式 clear
                int limit = 25;                          // ORIG 上限存槽 -0x20（setl 形）
                for (int j = 0; j < limit; )
                {
                    const int v = ScanInt(&ok);
                    if (!ok)
                        break;
                    values.push_back(v);
                    ++j;
                }
                out.m_map18.insert(std::make_pair(key, values));
            }
            goto scan_top;
        }

        if (token == "[quiz value]")
        {
            const int count = ScanInt(&ok);
            if (!ok)
                goto scan_done;
            for (int i = 0; i < count; )
            {
                int k = ScanInt(&ok);
                if (!ok)
                    goto scan_done;
                int v = ScanInt(&ok);
                if (!ok)
                    goto scan_done;
                out.m_map0.insert(std::make_pair(k, v));
                ++i;
            }
            goto scan_top;
        }

        if (token == "[reward]")
        {
            const int count = ScanInt(&ok);
            if (!ok)
                goto scan_done;
            for (int i = 0; i < count; ++i)
            {
                std::pair<int, int> reward;              // 默认构造（ORIG pair()）
                int a = ScanInt(&ok);
                if (!ok)
                    break;
                reward.first = ScanInt(&ok);
                if (!ok)
                    break;
                reward.second = ScanInt(&ok);
                if (!ok)
                    break;
                out.m_map30.insert(std::make_pair(a, reward));
            }
            goto scan_top;
        }

        if (token == "[use item]")
        {
            while (true)
            {
                const int v = ScanInt(&ok);
                if (!ok)
                    break;
                out.m_vec48.push_back(v);
            }
            goto scan_top;
        }
    }
    goto scan_top;
scan_done:
    return 0;
}