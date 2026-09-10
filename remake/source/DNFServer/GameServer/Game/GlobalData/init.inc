void WongWork::CSimpleSSO::init(MySQL* mysql)
{
    // ORIG 0x086027fc：保存 DB 句柄；逐块（行*0x6ba0 + 列*0x47c）以
    // strKeyTable_[row*24+col] 为密钥（keyLen=0x20）、CRijndael::sm_chain0 为
    // 初始链，调用 CRijndael::Initialize(…,0x10,0x10,0,0)。每块按 ORIG 步长
    // 0x47c 访问；当前 CRijndael::Initialize 写入范围 0x18..0x43c，块内安全。
    m_mysql = mysql;
    for (int row = 0; row < 31; ++row)
    {
        for (int col = 0; col < 24; ++col)
        {
            // [b28d-solo-GlobalData] P183 去指针缓存：ORIG 0x0860283x 每次重发
            // row/col 两级寻址直传 this；缓存 `CRijndael* r` 多 1 条且槽整体深 4
            // （row/col -0x14/-0x10 vs ORIG -0x10/-0xc）。
            // [b28d-solo-GlobalData] 索引加法序两形（row*24+col / col+row*24）实测 4.4
            // 恒折 `add -0xc(%ebp),%eax` 内存版，ORIG 4.1 先 `mov col,%ecx` 寄存器版
            // ⇒ P30/N4 add-vs-mem 封顶（余 1 条 + 槽深差 4）。
            // [G7-sub] 本轮复核：ORIG 0x08602823..0x08602834 `mov -0xc(%ebp),%ecx; …
            //   shl $0x3,%eax; add %ecx,%eax`（下标先入 ecx 的寄存器版）；我方同段为
            //   `add -0xc(%ebp),%eax`（内存版）。ORIG 45 / 我方 44（gap=1），
            //   p167 CALLSITE CLEAR（唯一 call 两侧各 1）⇒ 判 P30/N4 封顶，不再试形。
            m_keys[row][col].Initialize(strKeyTable_[row * 24 + col], 0x20,
                                        CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
        }
    }
