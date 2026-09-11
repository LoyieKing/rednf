// ORIG 0x0897c1c6（0x1d1 字节）逐分支转写：
//   以 g_independentDropScriptBaseDirectory 为根装载；记录索引=
//   FindIndexByFullScan(path)；[list] 段 (itemId,cumRate) 对累积进
//   info.m_list04；结束后 m_map24[索引]=(m_vec0.size(), 累积和) 并入 m_vec0。
// b28d-solo-DomainMiscDeps: DIFF 残余（p167 CLEAR 20/20；N82 槽分配）
bool IndependentDropParameterScript::importIndependentDropParameterScript(
    const char* path)
{
    if (!loadRDARScriptFile(g_independentDropScriptBaseDirectory, path))
    {
        return false;
    }
    std::string token;
    bool ok = false;
    IndependentDropListInfo info;
    info.m_field0 =
        g_independentDropScriptFileList.FindIndexByFullScan(path);
    int acc = 0;
    // P212/P309：ORIG 槽 itemId=-0x3c / rate=-0x40（声明序在 acc 之后）⇒ 头部声明。
    int rate;
    int itemId;
    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }
        // P222/P59 正向包裹：ORIG 0x0897c25f `test %al; je +0x65`（假→循环头，
        // 真直落内层）；`!=`+continue 反产 `je 内层; jmp 循环头`。
        if (token == "[list]")
        {
            while (true)
            {
                itemId = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                rate = ScanInt(&ok);
                if (!ok)
                {
                    return false;                  // 报告 0x0897c296 路径
                }
                // ORIG 0x0897c2a0：`mov acc,%edx; mov rate,%eax; lea (%edx,%eax,1),%eax;
                // mov %eax,acc`（4 条、unsigned 加法、两操作数均先载寄存器）。
                // [L3Free25to36 2026-09-11] `acc + rate` 形在 4.4.6 产 5 条
                // （`mov acc,%eax; mov %eax,%edx; mov rate,%eax; lea; mov`）=141 vs 142
                // （gap −1、DIFF）；**交换书写序为 `rate + acc` 后产 4 条
                // `mov rate,%edx; mov acc,%eax; lea (%edx,%eax,1),%eax; mov`**
                // ⇒ 141 vs 141（gap 0）且助记符序列与 ORIG 全等（分类 DIFF→NEAR）。
                // 唯一残差 = 两操作数寄存器互换（ORIG edx=acc，4.4 RHS-first edx=rate），
                // 属 4.1/4.4 lowering 差；如四数口径不接受 near+1/diff−1，改回
                // `acc + rate` 即可（代价：gap −1、DIFF）。
                acc = (int)((unsigned int)rate + (unsigned int)acc);
                info.m_list04.push_back(std::pair<int, int>(itemId, acc));
            }
        }
    }
    // ORIG 0x0897c2e8：`call size; mov %eax,-0xc(%ebp)` 单槽单写，随后
    // `lea -0xc(%ebp),%edx` 直传地址（-0xc 即 `unsigned int&&` 形参的**物化临时**）。
    // P0/P183：具名 `count` 槽会再产一份物化临时 ⇒ 我方 `mov %eax,-0x44; mov -0x44,%eax;
    // mov %eax,-0x10` 三条（ORIG 一条）⇒ 去掉具名局部，prvalue 直绑 rvalue 引用。
    m_map24[info.m_field0] =
        std::make_pair<unsigned int, int&>(m_vec0.size(), acc);
    m_vec0.push_back(info);
    return true;
}