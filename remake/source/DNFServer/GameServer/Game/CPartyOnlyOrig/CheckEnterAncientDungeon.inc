// ORIG T 0x0859e7fe（CheckEnterAncientDungeon_0859e7fe.md，单参别名版）：
// dungeon 未开组队入口时做 ancient 入场券检查；否则按 4 参事件版判定。
// 残差归因（N 类）：ORIG 将 itemIdx 读入 %esi 跨 getCurCharacInvenR 调用
// 缓存（4.4 恒为 call 后现场重读，P75 outgoing-arg 装载时刻族）、i 槽 -0xc
// 与 items 基 -0x111（4.1 帧内 4B 交错，P112）、循环头双 movl $0 初始化
// 复制（4.1 tail-duplicator 产物）均无法由合法源码复现。
// b28d-solo-CPartyOnlyOrig 判词：DIFF（部分修复，残余 N 封顶）@0x0859e7fe。
// 本轮修法：①`while (slots[i] == -1) { return 0; }` 复现 ORIG 0x0859e97c
// `cmp $-1; sete %al; test %al,%al; je`（4.4 的 `if` 直形恒折 `jne`，探针
// p22 g6）；②`int i = 0;` + `for (i = 0; ...)` 双初始化复现 ORIG 双
// `movl $0,-0xc`（P191）。残余 N 封顶：ORIG 第二处 `slots[i]` 读复用 %ebx
// （i 缓存）vs 4.4 恒重读 `-0xc(%ebp)`（+1 条，N7/N18a 寄存器分配族；register
// 提示反致槽漂移 -0x1c→-0x28，已回退）。**报告撞名警示：
// docs/class_func_reports/CParty/CheckEnterAncientDungeon.md 是 4 参重载
// 0x0859e528 的内容，本件 ORIG 基线取自二进制 nm 切片 0x0859e7fe..0x0859eac2。**
// 修法已落：`!= 1` 取反形复现 ORIG `xor $1; test; je`（P194；CheckEnterEventDungeon
// 返回 bool）；Main 代落 RequiredItem::m_enableFlag char→bool 后
// `!m_enableFlag` 段与 ORIG xor 形对齐。
// [b28d-round5 残余 N 复核 · 新钥匙逐条试过，均不可复现]
//  · 目标：ORIG 0x0859e93b `mov -0xc(%ebp),%ebx` 后，slots[i] 的写 0x0859e972
//    `mov %eax,-0x1c(%ebp,%ebx,4)` 与读回 0x0859e976
//    `mov -0x1c(%ebp,%ebx,4),%eax` **双用 %ebx**（我方读回处重载
//    `mov -0xc(%ebp),%eax`，+1 条 = N7/N18a 寄存器分配族）。
//  · P226 逗号/语句表达式（`while (slots[i]=f(), slots[i]==-1)`、`(…, slots[i])==-1`）
//    ⇒ 索引仍重载且跳向翻成 jne（探针 /tmp/probe_party/v.cpp g1/g5）——更差。
//  · 合并形 `while ((slots[i] = f()) == -1)` ⇒ 索引复用与 ORIG 同形、长度对齐，
//    但 4.4 折 `jne <return 岛>`（ORIG 4.1 为 `je`，fall-through = return 岛）
//    且 return 岛被移到循环尾 ⇒ 指令序与 ORIG 错开（探针 u.cpp f3 vs f4）——不采纳。
//  · 非合并形 `while (slots[i] == -1)` 是唯一保 `je` 同向 + 全序对齐的形（探针
//    u.cpp f4），残差恒为 1 条索引重载。⇒ 维持现形，N 封顶。
int CParty::CheckEnterAncientDungeon(CDungeon const* dungeon)
{
    Inven_Item items[4];
    int slots[4];
    unsigned char flag;    // ORIG -0x1d：items 顶与 slots 底之间声明于函数作用域
    memset(items, 0, sizeof(items));
    memset(slots, 0, sizeof(slots));

    if (const_cast<CDungeon*>(dungeon)->m_entryRestrictFlag)
    {
        flag = 0;    // ORIG +0x79 分支内清零
        // b28d-solo-CPartyOnlyOrig：ORIG +0x0a7 `xor $0x1,%eax; test %al,%al;
        // je <mov $1>`（P194 `!=1` 形；CheckEnterEventDungeon 返回 bool）。
        // 旧 `==1?1:0` 产 `test %al;je` 直跳（少 xor）。
        if (CheckEnterEventDungeon(dungeon, flag, items, slots) != 1)
        {
            return 0;
        }
        return 1;
    }

    if (dungeon->m_entryRequiredItem.m_itemIdx == -1)
    {
        return 1;
    }
    if (!dungeon->m_entryRequiredItem.m_enableFlag)
    {
        return 1;
    }
    if (m_battleField.m_bloodState == 1)
    {
        return 1;
    }
    // b28d-solo-CPartyOnlyOrig：ORIG 0x0859e8f8/0x0859e8ff 双 `movl $0,-0xc`
    // （4.1 声明初始化 + for 初始化各一条，P191 配套）。
    int i = 0;
    for (i = 0; i <= 3; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        // [b23S3C] P48 去缓存：ORIG 两用点各自 call getCurCharacInvenR
        // （+0x163 / +0x1b4 两次独立 call，结果 eax 直落实参槽无 inv 槽）。
        slots[i] =
            reinterpret_cast<CUserCharacInfo*>(m_member[i].m_pUser)
                ->getCurCharacInvenR()
                ->check_item_exist(dungeon->m_entryRequiredItem.m_itemIdx);
        // b28d-solo-CPartyOnlyOrig：ORIG 0x0859e97c 为 `cmp $-1; sete %al;
        // test %al,%al; je`（4.1 槽比物化）；4.4 的 `if` 直形恒折 `jne`，
        // `while (cond) { return X; }` 形逐条复现（P225 扩展，探针 p22 g6）。
        while (slots[i] == -1)
        {
            return 0;
        }
        items[i] =
            reinterpret_cast<CUserCharacInfo*>(m_member[i].m_pUser)
                ->getCurCharacInvenR()
                ->GetInvenSlot(1, slots[i]);
        if (items[i].m_addInfo2 < dungeon->m_entryRequiredItem.m_itemCount)
        {
            return 0;
        }
    }
    return 1;
}