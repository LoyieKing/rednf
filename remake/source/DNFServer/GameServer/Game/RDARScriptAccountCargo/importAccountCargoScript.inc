// b28d-solo-InitScriptProducers_1: DIFF→N 封顶（p167 20/20 CLEAR，语义闭合）。
// 本波复核（clean 归一后 132=132 指令，仅 3 处）：①我方多 1 条入口 jmp（ORIG
// `jmp+nop` 自跳转被归一剥离，P164 do-while 入口布局）；②ORIG do-while 条件
// `call operator==; test %al,%al; je <循环头>` 无 xor，4.4 恒插 `xor $0x1`
// （P320/S138：4.1 循环条件取反直接反转跳转）；③极性 je↔jne（由②派生）。
// 四形实测 `!()`/`==false`/`==0`/体内 break = 137/137/137/139 vs ORIG 135 均不可翻。
// ORIG T 0x088c7e80。段结构：[required level] / [required item] /
// [upgrade info](grade 与 UpgradeInfo 首字段同一栈位重叠，随后再扫三 int) /
// [account cargo end]。未识别 token 直接跳过；ScanInt 断流按 ORIG 精确分支处理。
bool importAccountCargoScript(AccountCargoScript* script, const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;

    // ORIG 0x088c7eac：script 先 spill 到具名指针局部 -0x10，之后全部经它访问（P0）。
    AccountCargoScript* s = script;
    s->clear();  // ORIG +0x032 调用 AccountCargoScript::clear(0x08372442)

    std::string token;
    int deadFlag = 0;                            // ORIG -0xc 只写一次的 int 局部
    (void)deadFlag;
    bool ok = false;

    do
    {
        while (true)
        {
            while (true)
            {
                while (true)
                {
                    if (!ScanType(token, true))
                        goto exitLoop;                      // ORIG +0x19d：以 ok 当前值返回
                    if (token == "[required level]")
                    {
                        s->m_reqLevel = ScanInt((bool*)0);
                        continue;                           // 回到 ScanType
                    }
                    break;
                }
                if (token == "[required item]")
                {
                    s->m_itemIdx = ScanInt((bool*)0);
                    continue;
                }
                break;
            }

            if (token == "[upgrade info]")
            {
                // ORIG +0xe4..+0x177：grade 扫描失败退出本段；其后三个 int 不做 ok 检查。
                // ORIG info 落 -0x58..-0x48 且 key 与 info.m_field0 同栈位（pair 构造两参
                // 同为 &local_58 实证）⇒ 无独立 grade 局部，直接用 info.m_field0。
                while (true)
                {
                    UpgradeInfo info;
                    info.m_field0 = ScanInt(&ok);
                    if (!ok)
                        break;
                    info.m_field4 = ScanInt(&ok);
                    info.m_field8 = ScanInt(&ok);
                    info.m_fieldC = ScanInt(&ok);
                    // b28d-lane25：P167 差集实证 ORIG 调 pair<int,UpgradeInfo>::C1<int&,UpgradeInfo&>
                    // 直构（P192），非 make_pair。
                    s->m_upgradeMap.insert(std::pair<int, UpgradeInfo>(info.m_field0, info));
                }
                continue;                                   // 段毕回主循环（do 底部仅对未知 token 生效）
            }
            break;
        }
    } while ((token == "[account cargo end]") == 0);         // ORIG do-while 判定

    // ORIG 0x088c8017：正常退出（命中 end tag）无条件置 ok=true 后返回。
    ok = true;

exitLoop:
    return ok;
}
