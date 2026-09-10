int ImportReturnUserRewardScript(const char* path, stReturnUserRewardScript& out)
{
    if (!loadRDARScriptFile("", path))
        return 0x36b4;

    std::string token;
    // [b28d-47] ORIG 0x0891813b/+0x45：`movb $0x0,-0xd`（死 bool 局部，P232）
    // 与 `movl $0x0,-0xc`（ret）预清，声明序 flag→ret。
    bool flag = false;
    (void)flag;
    int ret = 0;
    out.m_levelUp.clear();

    // ORIG 0x089180ea 单层 for(;;)：三处回边（[pacakege item]/非 [reward equip item]/
    // ret==0）都回同一 ScanType 头；ScanType 失败落**尾部岛** `nop; mov $0,%ebx`
    // （P223 岛 + P164 尾标签），故写 goto 尾标签而非就地 return。
    for (;;)
    {
        if (!ScanType(token, true))
            goto scan_fail;
        if (token == "[pacakege item]")
        {
            ScanInt(&out.m_field0);
            ScanInt(&out.m_field4);
            continue;
        }
        if (token == "[reward equip item]")
        {
            ret = LoadRewardDataByGrowType(out.m_levelUp, "[/reward equip item]");
            // ORIG 尾 `cmpl $0,-0xc; je <循环头>`（直跳）⇒ 写 break 形；`while(ret==0)`
            // 4.4 反产 `sete %al; test %al; jne` 物化（多 2 条）。
            if (ret != 0)
                break;
        }
    }
    return ret;
scan_fail:
    return 0;
