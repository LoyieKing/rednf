bool importDBLogItem(char* path, std::vector<stDBLogItem>* out)
{
    // ORIG T 0x0890dbba（204 条；单出口 ebx 形——4.1 把所有失败/EOF 路径 merge
    // 到共享 dtor+return 链，4.4 的多 return 形会复制 3 组 string dtor ≈ +52。
    // 本形以 result 局部 + goto done 复现共享出口）。外层每轮消耗两个节头：
    // head1=[dblog buyitem]（flag 0）内层循环、head2=[dblog getitem]（flag 1）
    // 内层循环；内层 ScanInt 断流回下一节头/主循环顶；ScanStr/时间转换失败
    // 整体 false；两处 EOF 均 true。ConvertStringToTime 的 by-value 串参以块级
    // 临时物化、dtor 先于结果测试（ORIG +0x106..+0x11b 序）。
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    // b28d-solo-InitScriptProducers_6: P171 死写入照抄 — ORIG 0890dc03
    // `movl $0x0,-0x14(%ebp)`（prologue 后零写、全函数无读取）。声明序按
    // ORIG 槽浅→深（4.4 先声明占浅槽，P212/P331）：deadA(-0x14)→token(-0x20)
    // →str(-0x24)→ok(-0x25)；result 为 4.4 共享尾的替代物（ORIG 用 ebx 承载），
    // 声明在最后使其落最深槽、不挤占上述 ORIG 槽位。
    std::string token;                 // ORIG -0x20
    std::string str;                   // ORIG -0x24
    int deadA = 0;                     // ORIG -0x14（列于两 string 之后，ORIG 0890dbfc）
    (void)deadA;
    bool ok = false;                   // ORIG -0x25
    // [C03 实测] ORIG 用 %ebx 承载返回值（无栈槽）：`mov $0x1,%ebx` 落在**循环之后**
    // （0x0890de2d），prologue 无初始化 ⇒ 声明不带初值、EOF 处统一赋值（裸 register
    // 提示合法，P231/P307；非 asm 绑定）。
    register bool result;

    for (;;)
    {
        if (!ScanType(token, true))
        {
            goto eof;
        }

        if (token == "[dblog buyitem]")
        {
            // b28d-solo-InitScriptProducers_6: P171 — ORIG 0890dc43
            // `movl $0x0,-0x10(%ebp)`（分支入口零写、循环回边不经过、无读取）。
            int deadB = 0;
            (void)deadB;
            for (;;)
            {
                stDBLogItem item;                  // ORIG memset(&item,0,0xc)
                memset(&item, 0, sizeof(item));
                // b28d-solo-InitScriptProducers_6: P0/P183 — ORIG 0890dc70
                // `mov %eax,-0x34(%ebp)` 直存字段（无具名 id 槽往返），ScanInt
                // 结果内联进字段赋值。
                item.m_itemIdx = static_cast<unsigned int>(ScanInt(&ok));
                if (ok != true) { break; }
                if (!ScanStr(&str))
                {
                    result = false;
                    goto done;
                }
                // b28d-solo-InitScriptProducers_6: P168a — ORIG 0890dcc5
                // `mov %eax,%ebx; xor $0x1,%ebx`（bool 取反入 ebx 跨 dtor 保活，
                // 无 bool 槽）⇒ 裸 `!ConvertStringToTime(...)` 形。
                if (!ConvertStringToTime(str, item.m_time))
                {
                    result = false;
                    goto done;
                }
                item.m_value = 0;                  // +0x08 memset 冗余重写 0
                out->push_back(item);
            }
        }

        if (!ScanType(token, true))
        {
            goto eof;
        }

        if (token == "[dblog getitem]")
        {
            // b28d-solo-InitScriptProducers_6: P171 — ORIG 0890dd55
            // `movl $0x0,-0xc(%ebp)`（分支入口零写、无读取）。
            int deadC = 0;
            (void)deadC;
            for (;;)
            {
                stDBLogItem item;
                memset(&item, 0, sizeof(item));
                // b28d-solo-InitScriptProducers_6: 同 buyitem（ORIG 0890dd82 直存）。
                item.m_itemIdx = static_cast<unsigned int>(ScanInt(&ok));
                if (ok != true) { break; }
                if (!ScanStr(&str))
                {
                    result = false;
                    goto done;
                }
                if (!ConvertStringToTime(str, item.m_time))
                {
                    result = false;
                    goto done;
                }
                item.m_value = 1;                  // +0x08 写 1
                out->push_back(item);
            }
        }
    }
eof:
    result = true;
done:
    return result;
