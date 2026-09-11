// ============================================================================
// CParty::onBloodClear —— ORIG 0x85b7c9e：血之副本单轮结算日志与成败回调。
// ============================================================================
void CParty::onBloodClear(bool flag, int param)
{
    // b28d-49 判词：N 封顶（N82 帧 0x30/0x40 + m_padResult 为 char 数组致 &m_padResult 基址 add；
    // 类型化 m_padResult 影响 C1Ev/D1Ev，留 Main 统一裁决）。P167 CLEAR。@0x85b7c9e
    // [G7-sub 复核] 本轮 diff 逐条核对：唯一差异 = ORIG 085b7cc1 `mov %eax,-0x1c(%ebp)` +
    // 085b7cc9 `mov -0x1c(%ebp),%eax`（GetMapPlayingTime 结果**占槽**）vs 我方
    // `mov %eax,%ebx`（寄存器保活）；帧 0x20(OURS) vs 0x30(ORIG)。前轮已试 3 形
    // （具名 unsigned / (unsigned) 强转 / 1000U）均被 4.4 无符号除法 lowering 折出
    // scratch 副本（帧 0x40 反劣）⇒ 维持 N92 封顶，本轮不重复试形。
    if (flag)
    {
        // b28d-solo-CParty2 试形记录（S1 一次旋钮已用尽，判 N92）：
        // ORIG 0x085b7cc9 `mov -0x1c(%ebp),%eax; mov $<A>,%edx; mul %edx` —— 被除数
        // 直取槽值、无副本。三形实测同果（均产 `mov %eax,-0xc; mov -0xc,%eax;
        // mov %eax,-0x2c; …mul` 双槽 + 帧 0x40）：
        //  ① `unsigned int playTime; playTime / 1000`
        //  ② `int playTime; (unsigned int)playTime / 1000`（P66）
        //  ③ `unsigned int playTime; playTime / 1000U`
        // ⇒ 属无符号除法求值固有差（N92 族），保持真型/直证形。
        // 试形记录（已回滚）：int 具名局部（P66 形）与 unsigned 同产 scratch 副本
        //   `mov %eax,-0xc; mov -0xc,%eax; mov %eax,-0x2c`（我方帧 0x40 vs ORIG 0x30）。
        // [2026-09-10 试形] ORIG 0x085b7cc4 `mov $<A>,%edx` 在 `mov -0x1c(%ebp),%eax`
        // **之前**且无 scratch 副本；4.4 无符号除法 lowering 额外 materialize 被除数
        // （`mov -0xc,%eax; mov %eax,-0x2c`）= 帧 0x40 vs ORIG 0x30。裸 register 存储类
        // 提示（P231/P307，非 asm 绑定、P0 合规）改变分配偏好。
        // [2026-09-10 试形②] volatile 版实测**劣化**（90 条/帧 0x40，多出 -0xc↔-0x1c
        // 双槽与 -0x2c scratch）⇒ 回退 register 版（88 条 = ORIG 条数、帧 0x20）。
        // 余差 = ORIG 槽读形（`mov %eax,-0x1c; …; mov -0x1c(%ebp),%eax; mul`）vs
        // 4.4 寄存器保活形 ⇒ N82/N126 帧族封顶（delta 4/2 条）。
        m_member[param].m_pUser->m_historyLog.MapClear(
            // b28d-solo-CParty：ORIG 085b7cd8 `movzwl 0x183e(%eax)` 单条 disp ⇒
            // 走 CParty.h 的 m_resultView 具名视角（P235），不再对 pad 取址 cast。
            m_gameResultSet.m_field4a,
            // 试形记录（已回滚）：`/1000`（int 常量）与 `/1000U` 同产 scratch 副本
            //   `mov %eax,-0xc; mov -0xc,%eax; mov %eax,-0x2c; …mul`（+2）⇒ 判 N92 族。
            GetMapPlayingTime() / 1000, 0);
        // ORIG 085b7d1f `movzbl 0x89f(%eax)` = CDungeon::m_bloodDungeonType
        //（unsigned char 宽度修正：原 *(char*) 为 movsbl 异形）
        if (m_battleField.m_pDungeon->m_bloodDungeonType == 1)   // 血图进度
        {
            APSystem::CUserProc::ClearActionAndSendtoUser(
                m_member[param].m_pUser, (APSystem::_ActionGroupIndex)0x1a, 0,
                (ENUM_PACKETCLASS)0);
        }
        OnSuccessedDungeonClear(m_member[param].m_pUser);
    }
    else
    {
        OnFailedDungeonClear(m_member[param].m_pUser);
    }
}