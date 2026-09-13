// ORIG T 0x08a65da5 _Z15ScanRewardItemsRSt6vectorI14RewardItemInfoSaIS0_EESs。
// [b14 报告核对·两条真修复]
//  ①真语义缺口（报告 Ghidra C 0x8a65e6a 段 + objdump 0x8a65e6a..0x8a65eec 双证）：
//    ORIG 为 `if (ok1) { if (s=="[job]") { ScanInt→m_job; ScanInt→m_growType; }
//    ScanInt→v; } info.m_count = v;` —— 最后一次 ScanInt 在 ok1 分支内**无条件**
//    执行（非 "[job]" 时 `je 0x12e` 亦落该段）；原实现写在 else 分支 ⇒ [job]
//    情形少消耗一个 int（真 bug）。修后 123=123、gap 0。
//  ②ORIG 全程只用一个 -0xc 变量（v 兼作 ScanInt 中转槽；Ghidra local_10 贯穿）
//    ⇒ 删除原 v2 具名中转。
// 声明序按 4.4「后声明更深」排：info@-0x20 / s@-0x24 已与 ORIG 一致。
// [b14 本波·DIFF→NEAR] `s == token` 真块位置镜像已翻正（旧判词②作废）：
//   ORIG 形 `test %al,%al; jne <真岛 +0x164>` + fall-through `mov $0x0,%ebx`
//   （false 紧随），if-return/`!=` 两形在 4.4 恒把真块前置 ⇒ 改 **goto 尾标签**
//   形（`if (s == token) goto scan_reward_true; return false;` + 函数尾
//   `scan_reward_true: return true;`），4.4 jump-threading 折成 ORIG 的
//   `jne <真岛>`；现 124=124 助记符逐条全等、全部分支偏移一致
//   （+0x76/+0x164/+0x187 三处与 ORIG 逐位相同）⇒ **NEAR**（ae 800→799、
//   diff 13→12、near 0→1）。
// 残余（NEAR，N 封顶）：标量区槽基 4 字节差（我方 ok1/unusedFlag/ok2/v =
//   -0xa/-0x9/-0xb/-0x10 对 ORIG -0xf/-0xe/-0xd/-0xc；ORIG 侧 int v 浅于
//   bool 组、4.4 侧 int 恒最深 = N82附/N126 版本分区常数；前置 dead pad 实测
//   ndiff 44→66 反劣、v 提前声明致 info/s 下移 4，均已回退）。
// p167 CLEAR（13=13）。
bool ScanRewardItems(std::vector<RewardItemInfo>& out, std::string token)
{
    // ok2 无初始化式（ScanTypeOrInt 写入后即读）；ORIG 预清仅两条
    // （`movb $0,-0xe; movb $0,-0xf`）。声明序三形实测见函数头判词。
    // [L13-b14 判据⑩ 槽序反转律试验] ORIG 槽深→浅 = ok1(-0xf)/unusedFlag(-0xe)/
    // ok2(-0xd)/v(-0xc)；按「深=先声明」把 ok1 提到声明序首位（形 A：123=123，
    // info/s 槽 -0x20/-0x24 与 ORIG 一致）。形 B（v 提前）实测 info/s 整体下移 4
    // （-0x24/-0x28）⇒ 回滚。残余 = 4.4 对 int(v)/bool 分组方向与 4.1 相反
    // （v 恒最深 -0x10 vs ORIG -0xc）⇒ N82/N126 版本槽基差封顶。
    // [P1T1 本波复核] 现状 = **NEAR**（124=124，助记符逐条相同，仅 -0x9/-0xa/-0xb/
    // -0x10 与 ORIG -0xf/-0xe/-0xd/-0xc 的槽号差；fdiff 16 块全为槽号替换）。
    // 4.4 对 bool 组与 int(v) 组按**相反**方向分组（我方 bools 浅/v 深），
    // 形 A/B 已实测（形 B 令 info/s 下移 4 更劣）⇒ 无新源形，维持 NEAR 不回退。
    bool ok1 = false;
    bool unusedFlag = false;
    bool ok2;
    int v;
    RewardItemInfo info;
    std::string s;
    for (;;)
    {
        v = ScanTypeOrInt(ok1, s, ok2, 1);
        if (ok1)
        {
            // [b14 本波 P325/P223 试验] ORIG 形 `test %al,%al; jne <真岛>` +
            // fall-through `mov $0x0,%ebx`（false 紧随；真岛在函数后部 +0x164）。
            // if-return / `!=` 两形实测均把 true 块前置（N93附 跨版本块序反差）
            // ⇒ 改 goto 尾标签形，让 4.4 走 `je <跳过>; jmp <尾标签>` 并由
            // jump-threading 折成 ORIG 的 `jne <真岛>`。
            if (s == token)
                goto scan_reward_true;
            return false;
        }
        if (!ok2)
            return false;
        info.m_itemIdx = v;
        info.m_job = 0xb;
        // [b57FIX · ScanStrOrInt 的两个输出标志用反了（拿 isStr 当 isInt 判）]
        // ORIG 08a65e3d..08a65e76 实参布局（objdump 逐条）：
        //   (%esp)=isStr ← **-0xe**（那个全程只被测试、不参与其它判断的 unusedFlag 槽）
        //   0x4(%esp)=s ；0x8(%esp)=isInt ← **-0xd**（即 ok2）；0xc=consume=1
        //   `call _Z12ScanStrOrIntRbRSsS_b` → `mov %eax,-0xc(%ebp)`(v)
        //   08a65e61 `movzbl -0xd; xor $1; test; je 08a65e76` ⇒ **`if (!ok2) return false;`
        //     判的是 isInt**（DECIMAL/HEX/FLOAT/STRING/PAIR_INT 都会置 isInt=1）
        //   08a65e76 `movzbl -0xe; test %al,%al; je 08a65eec` ⇒ `if (unusedFlag) { … }`
        //     即 isStr 分支：s=="[job]" 读 m_job/m_growType，末尾 08a65ed3 **无条件**
        //     再 ScanInt 一次，08a65eec 是共享的 m_count 赋值尾。
        // 旧形写 `ScanStrOrInt(ok2, s, ok1, 1)`（isStr→ok2、isInt→ok1）却仍
        // `if (!ok2) return false;` ⇒ **拿 isStr 当 isInt 判**：奖励项第 2 个 token
        // 是整数时 isStr=0 ⇒ 直接 return false。
        // 实机 A/B 实证（Title/achievement_34_title.qst，Frida /tmp/ab_qq_{ours,orig}.js
        // 对钩 getToken/getType/ScanType）：同一 token 流前 32 词逐词一致；第 33/34 词
        // 是两个 DECIMAL，ORIG 继续读到第 35 词 `[/reward int data]`（无 getType ⇒ 由
        // ScanTypeOrInt 消费）后返回 true 并走完 50 词（END ret=1）；我方在第 34 词后即
        // END ret=0 ⇒ `importQuestScript` 返回 0 ⇒ `QuestList::load_list` 第 1 个文件
        // 即失败 ⇒ `Loading Quests Fail`。
        v = ScanStrOrInt(unusedFlag, s, ok2, 1);
        if (!ok2)
            return false;
        if (unusedFlag)
        {
            if (s == "[job]")
            {
                if (!ScanInt(&v))
                    return false;
                info.m_job = v;
                if (!ScanInt(&v))
                    return false;
                info.m_growType = v;
            }
            if (!ScanInt(&v))
                return false;
        }
        info.m_count = v;
        out.push_back(info);
    }
scan_reward_true:
    return true;
}
