// ----------------------------------------------------------------------------
// 3. ORIG T 0x0868745e sz 0x269。
// 等级奖励邮件：CLevelRewardTableMgr 开启且表就绪、非挑战职业时按 (jobIdx,
// levelIdx) 取 itemNo/count，组 Inven_Item 后经 ReqDBSendNewSystemMail 投递
// （itemNo==0 仅金额走第三参；==1 固定道具直写字段后 ResetItemAttr；其余经
// find_item 的虚槽 2 make_item 复制并 set_add_info）。
void CUser::_processLevelReward(unsigned int level, unsigned int characNo,
                                char job)
{
    // b28d-solo-CUserDepsB: 语义全还原（含两处真 bug 修正：①IsChallengingJob
    // 判定反——ORIG 0x086874cb/Ghidra C 均 `if(!f()) return`；②else 分支
    // m_addInfo 写序在 make_item 之前，ORIG 0x0868761a）。
    // [SweepCUserDepsB 本轮翻正] 旧判「N95 局部槽基差 4 封顶」**作废**：
    // 真因是本函数尾部把 GetServerGroup() 存进具名局部 `ENUM_SERVER_GROUP
    // serverGroup`（多占 -0x10 槽 ⇒ 其后全部标量槽号整体 -4 平移，伪 N95）。
    // ORIG 0x086875xx 段 `mov %eax,%ebx` 证明 serverGroup 走 callee-saved
    // 跨 strlen 保活、**无槽往返** ⇒ 按 P0/P329 内联回实参位后：
    // 160(ORIG) vs 163，全函数 0..155 行逐条全等（槽号 -0x28/-0x24/-0x20/
    // -0x1c/-0x18/-0x14/-0x10/-0xc/-0x7a/-0x3d/-0x17a 与 ORIG 完全一致）。
    // [F3b-C05③ 2026-09-10 翻案 ⇒ AE（164=164 逐条全等）] 旧「N121 尾部
    // return 中继岛封顶」判词**作废**：L29 命中的 3 处 REAL（ORIG 0x086874a8
    // table==0 / 0x08687508 levelIdx<0 / 0x08687512 jobIdx<0 三条
    // `je/js <epilogue +0x260>` 直达）不是 4.1/4.4 出口块分配差，而是平铺
    // `if (c) return;` 在 4.4 各产一组 nop+jmp 中继岛。改为**正向包裹**
    // （`if (table != 0) { … }` / `if (levelIdx >= 0 && jobIdx >= 0) { … }`）
    // 后 4.4 把 fail 路径 jump-thread 直达共享尾出口 ⇒ 尾岛 7→4、ours
    // 175→164（= ORIG 164）、本件 DIFF→AE。其余 4 个 return（IsOn/
    // IsChallengingJob/itemCount/script）与 ORIG 同为岛，保持原形不动。
    // 旧探针「嵌套正向包裹=0 桩 / 掉 xor 物化」作废的原因：当时把
    // `if (!mgr->IsOn()) return;`（含 xor $1 物化条）也一并嵌套（越界改写）。
    // p167 CLEAR（20/20，仅 C1/C2 同址别名噪音）。ORIG 0x0868745e。
    CLevelRewardTableMgr* mgr = CLevelRewardTableMgr::GetInstance();
    CLevelRewardTable* table = 0;                  // local_28 初值 0
    if (!mgr->IsOn())   // P292 档①：IsOn 已按调用点 xor 形改 bool 返回（P9 直返形
                        // 不变），此处直接产 ORIG 的 xor $1;test;jne。
        return;
    table = mgr->GetProperLevelRewardTableR();     // T 0x08687e62
    // [F3b-C05③ 2026-09-10] ORIG 0x086874a8 `je <epilogue +0x260>` 直达尾 ⇒
    // 平铺 `if (table == 0) return;` 会产一组 nop+jmp 中继岛；改正向包裹复现。
    if (table != 0)
    {
        int rewardItemNo = 0;                      // ORIG -0x20 初值 0（声明在
                                                   // IsChallengingJob 之前）
        // 真语义 bug 修正：ORIG 0x086874cb `call IsChallengingJob; xor $1; test; jne
        // 早退岛` 与 Ghidra C `if (cVar2 == 1) { 主体 }` 均表明返回**真**才继续；
        // 旧写 `if (IsChallengingJob(job)) return;` 语义反。
        if (!mgr->IsChallengingJob(job))           // T 0x08687792
            return;
        int jobIdx = table->GetProperJobIndex(job);    // T 0x08687e78
        int levelIdx = table->GetProperLevelIndex(level);  // T 0x086876c8
        // [F3b-C05③ 2026-09-10] ORIG 0x08687508/0x08687512 两条 `js <epilogue
        // +0x260>` 直达同一尾标签（相邻同目标）⇒ 正向 && 包裹复现；平铺两条
        // `if (…) return;` 会各产一组 nop+jmp 中继岛（+4 条）。
        if (levelIdx >= 0 && jobIdx >= 0)
        {
            rewardItemNo = table->GetRewardItemNo(jobIdx, levelIdx);  // local_24
            int itemCount = table->GetItemCount(jobIdx, levelIdx);    // local_18
            if (itemCount < 0)
                return;
            char title[21];                            // local_41（21 字节槽）
            strncpy(title, table->GetMailTitle(levelIdx), 0x14);
            char body[256];                            // local_17e
            strncpy(body, table->GetMailString(levelIdx), 0xff);
            Inven_Item item;                           // local_7e（W ctor）
            int sendCount = 0;                         // local_14
            if (rewardItemNo == 0)
            {
                sendCount = itemCount;                 // 仅金额，道具保持默认构造
            }
            else if (rewardItemNo == 1)
            {
                // ORIG 0x086875d5..0x086875ec 写序：+2=1（立即数，条件已证 ==1）、
                // +1=2、+7=itemCount、+11=1(word)。
                item.m_addInfo = 1;
                item.m_field1 = 2;
                item.m_addInfo2 = itemCount;
                item.m_fieldb = 1;
                item.ResetItemAttr();                  // W 0x080cb892
            }
            else
            {
                // P231 纯 register（无 asm 绑定，P0 合规）：ORIG 0x086875f9
                // `mov -0x20(%ebp),%ebx` 跨 G_CDataManager 调用保活（callee-saved）；
                // 直接内联实参 4.4 用 %edx 中转且不 push %ebx（全帧浅 4）。
                register int itemNo = rewardItemNo;    // P231：值跨 G_CDataManager
                                                       // 调用保活于 %ebx（无绑定）
                CItem* script = G_CDataManager()->find_item(itemNo);  // T 0x0835fa32
                if (script == 0)
                    return;                            // je +0x25f 直接出口
                item.m_addInfo = rewardItemNo;         // ORIG 0x0868761a 先写再 make
                script->make_item(item);               // vtable+8 虚槽（CItem 槽 2）
                item.set_add_info(itemCount);          // W 0x080cb884
            }
            // [P0/P329 本轮] ORIG 0x086875xx 段把 GetServerGroup() 结果留 %ebx 跨
            // strlen 保活（行 131 `mov %eax,%ebx`），无具名局部槽 ⇒ 原码为实参位内联
            // 表达式；具名 `ENUM_SERVER_GROUP serverGroup` 多占 -0x10 槽并使全帧标量
            // 槽号整体 -4 平移（N95 假象）。
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                title, item, sendCount, characNo, body,
                static_cast<int>(strlen(body)), 0, GetServerGroup(), 0, 0);
        }
    }
}