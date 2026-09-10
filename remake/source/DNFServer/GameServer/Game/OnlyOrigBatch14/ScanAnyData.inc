bool ScanAnyData(int& type, std::string& str, int& value, bool ignoreUnknown)
{
    type = 3;
    int token;
    bool got = false;
    // 声明序 token/got：4.4 对函数头标量按「后声明更深」分配（本 TU
    // importActiveStatus/importHiddenNpcListInfo 同律复证）⇒ token@-0x10? 见
    // 实测 token@-0x14(深)/got@-0x11(浅) 方向已对齐 ORIG。
    for (;;)
    {
        token = g_dnfLex.getToken();
        // ORIG if 链布局（非 switch 表）；case 9 成功时 ORIG 将 token 置 7，
        // 使后续循环 break 检查（2/3/4/7/5/0）命中，避免继续循环。
        if (token == 2)   // decimal
        {
            value = g_dnfLex.getDecimal();
            type = 1;
            got = true;
        }
        else if (token == 3)   // hex
        {
            value = g_dnfLex.getHexDecimal();
            type = 1;
            got = true;
        }
        else if (token == 4)   // float
        {
            value = (int)g_dnfLex.getFloat();
            type = 1;
            got = true;
        }
        else if (token == 7)   // string
        {
            str = toTString(g_dnfLex.getString());
            got = true;
            type = 0;
        }
        else if (token == 5)   // type
        {
            str = toTString(g_dnfLex.getType());
            got = true;
            type = 2;
        }
        else if (token == 9)   // pair int
        {
            int key = g_dnfLex.getPairInt();
            if (g_dnfLex.getToken() == 0xa)
            {
                // ORIG 0x88bdcb8 `movl $0x7,-0x14(%ebp)` 紧接判 0xa 的
                // `je` 之后、findString 调用**之前** ⇒ 源码里 token=7 在
                // findString 之前（顺序照 ORIG 汇编）。
                token = 7;   // ORIG 0x88bdcb8 movl $0x7,-0x14(%ebp)
                str = g_scriptStringManager_.findString(key,
                                                        g_dnfLex.getPairStr(),
                                                        0);
                type = 0;
                got = true;
            }
        }
        else
        {
            got = false;
            type = 3;
        }
        if (token == 2 || token == 3 || token == 4 || token == 7 ||
            token == 5 || token == 0)
        {
            break;
        }
        // [L13-b14 复核·N122 复证] ORIG 0x88bdd51 段 `cmpb $0x0,-0x2c(%ebp); je`
        // （内存直测 2 条）；`if (!x)` 产 movzbl+xor$1+test+je（4 条）。
        // [P1T1 本波语义轴修复] ORIG 分支极性直证：0x88bdd55 `je 0x88bdb73`
        // （0x88bdb73 = 循环头 nop）；-0x2c 即第 4 形参 ignoreUnknown（0x88bdb5e
        // `mov 0x14(%ebp),%eax` → 0x88bdb61 `mov %al,-0x2c(%ebp)`）。
        // je 条件 ZF=1 = flag==0 ⇒ **flag==0 跳过 log、flag!=0 才 log** ⇒ 原码是
        // `if (ignoreUnknown) { log }`。旧译文 `if (!ignoreUnknown)` 为**反向行为**
        // （ignoreUnknown 为假时报错，为真时静默）⇒ 真语义缺口，本波翻正。
        // 附带 codegen 收益：4.4 对 `if (x)` 直测 bool 参数槽产 `cmpb $0x0,<槽>;
        // je <跳过>`（探针 /tmp/p1/probe_small.cpp），与 ORIG 逐条同形。
        if (ignoreUnknown)
        {
            LogManager::logFormat(0, "../RDARScript.cpp", __FUNCTION__, 0x746,
                                  "ERROR! %s - TYPE mismatch. %s:%d, token = '%s'",
                                  // [b14 报告核对·真语义 bug] 报告 Ghidra C
                                  // 0x88bdd77 段实参序为 ("ScanAnyData", streamName,
                                  // lineNo, tokenData)：原写 (streamName, "ScanAnyData")
                                  // 两串互换 ⇒ 日志%s 内容反写。
                                  "ScanAnyData",
                                  g_dnfLex.getStreamName().c_str(),
                                  g_dnfLex.getLastLineNo(),
                                  g_dnfLex.getLastTokenDataAsString().c_str());
        }
    }
    return got;
}
