// ORIG LoadScript 0x082f815c（0x2e2 字节）逐分支转写：返回出错行，0=成功。
// 结构按 ORIG：各键失败即 return（ebx=err 直落尾，无 err 汇聚变量）；
// value(-0x20)/ok(-0x21) 为函数级共享（key2/key4 复用同槽）；键1/3/5 循环
// `cmpl $9; setle` = i<=9 头检 for；key2 上界检查 ORIG 不存在（仅 <=0 →
// 0x3c）；key4 push_back 后存在 ok 二次重载检查（0x082f8386，别名后重读）；
// 未知键直接 return 0x64（0x082f8401），非 continue。
// [b28d-solo-CDataManager AE 208=208] 真语义 bug ×2 修复：① ORIG 0x082f8287 有
// `cmp $0xc8; jle` 上界检查（value>200 → 0x3c），旧码缺；② ORIG 0x082f82ac 写
// 地址 = this+0xc+(value+0x14)*4 = m_goldPerLevel[value]，旧码 `[value+0x17]`
// 多写 0x5c 字节越界。本轮翻正（o 208 / n 209→208，DIFF→AE）：
//   ① 外层 while 旋转 1 条 ⇒ goto 头条件形（P223/P283）；
//   ② `if (v<=0) return 0x3c; if (v>200) return 0x3c;` 两处独立 err 块被 4.4
//      各自内联 ⇒ 合并短路 `if (v<=0 || v>200) return 0x3c;` 复现 ORIG 共享
//      失败块（P258 短路物化方向）。
// p167 19/19 CLEAR。
int CAutoMarketConditionsControl::LoadScript(const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return 0x1e;
    }
    std::string token;
    int value = 0;
    bool ok = false;
    // [b28d-solo-CDataManager] ORIG 条件块在循环头（+0x42 直落）、尾部回跳；
    // 4.4 对 while(cond) 恒旋转成入口 jmp 尾条件（+1 条；while(true)+break
    // 形实测 our 210 更差）。改 goto 头条件形（P223/P283 族）复现。
load_cond:
    if (!ScanType(token, true))
        goto load_done;
    {
        if (token == "[control of gold drop amount rate]")
        {
            for (int i = 0; i <= 9; ++i)
            {
                m_goldRate[i] = ScanFloat(3, &ok);
                if (ok != true)
                {
                    return 0x2f;
                }
            }
        }
        else if (token == "[control of gold drop amount rate per level]")
        {
            memset(m_goldPerLevel, 0, 0x324);
            while (true)
            {
                value = ScanInt(&ok);
                if (ok != true)
                {
                    break;
                }
                // [b28d-solo-CDataManager 真语义 bug 修复] ORIG 0x082f8280-8c 有
                // 上界检查（`cmp $0xc8; jle` ⇒ value>200 也返回 0x3c），旧码缺。
                // 短路合并复现 ORIG `jle err; cmp; jle body; err:` 共享失败块
                // （P258 德摩根/短路物化方向；两个独立 if-return 在 4.4 会各自
                // 内联 err 块，our +3）。
                if (value <= 0 || value > 200)
                {
                    return 0x3c;
                }
                // [b28d-solo-CDataManager 真语义 bug 修复] ORIG 0x082f82ac 写地址
                // = this+0xc+(value+0x14)*4 = this+0x5c+value*4 = m_goldPerLevel
                // [value]（成员 +0x5c 实证）；旧码 `[value+0x17]` 多写 0x5c 字节
                // （23 个 int），越界破坏后续成员。
                m_goldPerLevel[value] = ScanInt(&ok);
                if (ok != true)
                {
                    return 0x40;
                }
            }
        }
        else if (token == "[control of equipment drop prob]")
        {
            for (int i = 0; i <= 9; ++i)
            {
                m_equipProb[i] = ScanFloat(3, &ok);
                if (ok != true)
                {
                    return 0x49;
                }
            }
        }
        else if (token ==
                 "[control of equipment drop prob per level range]")
        {
            // ORIG 0x082f81e9 `movl $0,-0x10`：key4 块首存在一个仅初始化
            // 的本地（后续未使用，P31 槽对齐实证保留）。
            int cnt = 0;
            (void)cnt;
            while (true)
            {
                value = ScanInt(&ok);
                if (ok != true)
                {
                    break;
                }
                m_vec3dc.push_back(value);
                // ORIG 0x082f8386：push_back（不透明调用）后 ok 重读判失败
                // → 0x57（别名性二次检查，忠实保留）。
                if (ok != true)
                {
                    return 0x57;
                }
            }
        }
        else if (token == "[control of durability decrease rate]")
        {
            for (int i = 0; i <= 9; ++i)
            {
                m_durRate[i] = ScanFloat(3, &ok);
                if (ok != true)
                {
                    return 0x60;
                }
            }
        }
        else
        {
            return 0x64;
        }
    }
    goto load_cond;
load_done:
    return 0;
}