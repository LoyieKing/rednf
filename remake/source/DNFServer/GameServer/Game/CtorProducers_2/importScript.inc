bool CharacManageScript::importScript(const char* path)
{
    // [SweepCtorProd2] 已修 3 个真 bug（p167 47=47 CLEAR，残余 4 项 C1/C2·D1/D2
    // 同址别名 = P93 豁免，nm 实证 ORIG 0x087065d0/0x08707be0/0x088e12f6/0x080c6d20）：
    // ① 8 处 `token == "…"` 实参序反写 → `"…" == token`（ORIG 每点
    //    `movl $"[…]",(%esp); mov %eax,0x4(%esp)` 选 `operator==(const char*,
    //    const string&)`；原写法选到另一重载 `…RKSb…EPKS3_`）；
    // ② `make_pair(key,val)` 的 key/val 去 const（ORIG 实例 `IRiS0_` 非 const 左值，
    //    const 版会实例化 `IRKiS1_` 另一符号）；
    // ③ `[exp bonus]/[gold bonus]` 分支由三目改为循环内 if/else-if + `else return false`
    //    （ORIG 0x88e0b8e..0x88e0be3：选表指针每轮重置、缺第 9 次 operator== 与失败返回）。
    // 帧 0x60→0x70（P232 撑帧，char framePad[0x10]）与 ORIG 对齐。
    // 残余 DIFF = while 布局（4.1 条件在前 fall-through vs 4.4 `jmp cond`）+
    // bool 判断 xor 物化（P168a④/N122：4.4 无 `movzbl;xor$1;test;je` 形）+
    // 解析段槽分配（N82/N93 族）⇒ 语义等价、结构 N 封顶。
    // ORIG 0x088e0968：RDAR 装载失败 ret0；分节扫描 [point bonus]/
    // [manage level point]/[manage level max]/[exp bonus]/[gold bonus]/
    // [manage basic explain]/[manage option]/默认 ContentsOptionRevision 行。
    // [M5F_CtorDB 2026-09-11] our<ORIG（342/329）⇒ 只找「ORIG 有我方无」段；
    // seq-align 差集**完全解析**为 13 条（无一条语义指令）：
    //   ① 4 处 `movzbl ok; xor $0x1,%eax; test %al,%al; je`（4.1 对 `if(!ok)` 先
    //      取反再物化；4.4 直产 `test; jne`）⇒ 每处差 2 条 = 8；
    //   ② 4 条 `nop`（4.1 循环对齐伪指令，落点见 0x088e0a9f/0x088e0c8a/0x088e0d1e/
    //      0x088e0d6f，全部不可达）⇒ 4；
    //   ③ 1 条 while 布局差（4.1 条件在头 + 回边 jmp +0x46，4.4 条件在尾 + `jmp +0x414`）。
    // 新钥匙逐条试过：P333（局部 static 双检锁表）不适用——本件无 .bss 取址/guard；
    // P337 死块——上述 nop 岛**不可达但无指令**（纯对齐），补 nop 无意义；
    // P283/P164 循环落点——`while (ScanType(token,true))` 4.4 恒尾测，改 for 同码。
    // 调用点差集 CLEAR 47=47（父泳道复核）；串/变参序（ERROR [%s][%s], path, token）
    // 与 ORIG 一致（§6.5 已核）。⇒ N 封顶（4.1 xor 物化 + 对齐 nop），语义等价。
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;
    std::string value;
    bool ok = false;
    char framePad[0x10];                         // [SweepCtorProd2] ORIG 帧 0x70 vs 我方 0x60
    (void)framePad;                              // ⇒ 全槽平移 0x10（P232 撑帧实验）
    while (ScanType(token, true))
    {
        // [SweepCtorProd2] 实参序判据（P18/P23 族）：ORIG 每个比较点都是
        // `movl $"[...]",(%esp); mov %eax,0x4(%esp)` ⇒ 字面量在 arg0、token 在 arg1，
        // 选 `operator==(const char*, const string&)`（_ZSteq…bPKT_RKSbIS3_T0_T1_E，
        // ORIG 9 次）；写反成 `token == "…"` 选另一重载（我方原 8 次）⇒ 全部翻正。
        if ("[point bonus]" == token)
        {
            while (true)
            {
                ManagePointBonus bonus;              // ORIG 栈 ctor 0x088e12f6
                bonus.m_field00 = ScanInt(&ok);
                if (!ok)
                {
                    break;                           // 回 ScanType 主循环（ORIG jmp +0x46）
                }
                bonus.m_field04 = ScanInt(&ok);
                if (!ok)
                {
                    return false;
                }
                bonus.m_field08 = ScanInt(&ok);
                if (!ok)
                {
                    return false;
                }
                m_pointBonus.push_back(bonus);
            }
        }
        else if ("[manage level point]" == token)
        {
            int level = 0;
            while ((level = ScanInt(&ok)), ok)       // do-while 形态（asm +0x11a）
            {
                m_manageLevelPoint.push_back(level);
            }
        }
        else if ("[manage level max]" == token)
        {
            m_manageLevelMax = ScanInt(&ok);         // +0x48
            if (!ok)
            {
                return false;
            }
        }
        else if ("[exp bonus]" == token || "[gold bonus]" == token)
        {
            // [SweepCtorProd2] ORIG 0x88e0b42/0x88e0b8e..0x88e0be3：
            // ① val(-0x50) 在循环**外**预清 `movl $0,-0x50`，循环内 `val = ScanInt`；
            // ② key(-0x4c) 每轮新声明；③ 选表指针(-0x10) 在**循环体内**每轮重置：
            // `== "[exp bonus]"` → +0x18，否则 `== "[gold bonus]"` → +0x30，
            // 否则 `mov $0,%ebx; jmp` = return false（原三目写法把这 2 次比较移到
            // 循环外 ⇒ p167 少 1 次 operator==、少失败返回路径）。
            int val = 0;
            while (true)
            {
                // ORIG make_pair 实例 = `IRiS0_`（int&，非 const 左值）⇒ key/val 必须
                // 是非 const 局部（P61/P315：const 左值会实例化 IRKiS1_ 另一符号）。
                int key = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                val = ScanInt(&ok);
                if (!ok)
                {
                    return false;
                }
                std::map<int, int>* table = 0;
                if ("[exp bonus]" == token)
                {
                    table = &m_expBonus;             // +0x18
                }
                else if ("[gold bonus]" == token)
                {
                    table = &m_goldBonus;            // +0x30
                }
                else
                {
                    return false;
                }
                const std::pair<std::map<int, int>::iterator, bool> ins =
                    table->insert(std::make_pair(key, val));
                if (!ins.second)                     // insert.second==0 → 查重告警
                {
                    std::printf("ERROR [%s][%s] key duplicated\n",
                                path, token.c_str());
                }
            }
        }
        else if ("[manage basic explain]" == token)
        {
            m_basicExplain.clear();                  // 先 clear 再按行号写入
            int index = 0;
            while (ScanStr(&value))
            {
                m_basicExplain[index] = value;
                ++index;
            }
        }
        else if ("[manage option]" == token)
        {
            m_manageOption.clear();
            while (true)
            {
                const int key = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                m_manageOption[key] = ScanInt((bool*)0);   // ORIG ScanIntPb(NULL)
            }
        }
        else
        {
            m_optionRevision.clear();
            while (ScanStr(&value))
            {
                ContentsOptionRevision rev;          // POD {int type; int rev;}
                rev.m_type = ContentsType::getType(value.c_str());
                rev.m_field04 = ScanInt((bool*)0);
                m_optionRevision.push_back(rev);
            }
        }
    }
    return true;                                     // ScanType 耗尽 ret1
}