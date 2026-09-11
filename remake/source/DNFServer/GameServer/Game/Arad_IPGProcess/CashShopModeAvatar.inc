// ---------------------------------------------------------------------------
// Arad_IPGProcess
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// CashShopModeAvatar —— ORIG T 0x0819c2b4（报告 CashShopModeAvatar.md 全文转写）。
// ---------------------------------------------------------------------------
// [b28d-solo-Arad_IPGProcess] NEAR（助记符序列全等，仅 len 槽号差）：①gift 分支原已全等；
// ②删具名 inven/characNo 改 receiver 链式（P52/P0）后 slot=-0x20、msg=-0x1c 与 ORIG 一致；
// ③`int len` 头部声明得 -0x24，ORIG 为 -0x38（4.1 外层标量与形参 spill 区 -0x2c..-0x34
// 连续分配 vs 4.4 具名区 -0x1c..-0x24 分区分配）⇒ N82/N93 槽分配族封顶；
// size_t 化实测反劣（NEAR→DIFF）。
// [S3/M3FreeNext15B 2026-09-11 复扫] ①§6.5 串审计 CLEAR（1 kinds/1 refs：
//   game_server_msg_104 @我方 .rodata+0）；②P339 试验：`const char* msg;` 提函数头
//   （全头部声明、ORIG 槽深序 len>msg）⇒ 复编后四数 12/28/2/1 与槽分布**逐值不变**
//   （-0x1c×3 / -0x20×3 / -0x24×2 / -0x2c×4 / -0x30×4 / -0x34×2）⇒ 已回滚；
//   ③P331 不成立（头部标量仅 len 一项，相对槽序与 ORIG 同向，非倒置）。
//   ⇒ 维持 N82/N93：ORIG len 属形参 spill 相邻深区 -0x38（4.1 函数级标量区），
//   4.4 具名区自 -0x1c 起（msg -0x1c / slot -0x20 / len -0x24），两侧帧同为 0x5c。
void Arad_IPGProcess::CashShopModeAvatar(CUser* user, unsigned long itemIdx,
                                         int a3, int a4, char c1, char c2,
                                         char const* str, int& out,
                                         unsigned int accId, int a10, char buyFlag)
{
    (void)a3;  // ORIG p3 在两分支均未使用（ebp 0x14 无读取）
    // [P6AuditOldN 2026-09-11 复核·维持 NEAR] 本件已试新钥匙：①P339 声明序（本处 len 置于
    // 函数头 = 声明序最前，我方槽 -0x24 为三个局部中最深 ⇒ 声明序已与 ORIG 同向）；
    // ②「声明即初始化」变体（`int len = (int)strlen(msg);` 移入 if 块）⇒ 实测把 **slot**
    // 从 -0x1c 推深到 -0x24（diff 由 len 单处变为 slot 单处，仍 NEAR）⇒ 已回滚。
    // 残差定性（ORIG 0x0819c2b4 直读）：ORIG 局部区自 -0x38 起（len 最深），我方自 -0x24 起
    // ⇒ ORIG 在 [-0x38,-0x24) 另有 0x14 字节被占（20B，无对应 store 指令 ⇒ 非我方漏建模的
    // 具名局部，属 4.4 内部命名临时），无源码旋钮 ⇒ N 封顶。
    int len;   // ORIG -0x38（形参 spill 区之后）⇒ 头部声明（4.1 先声明深）

    if (buyFlag == 1)
    {
        // mangled _processAvatarGift(CUser*, i, i, c, c, PKc, j, i)
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processAvatarGift(
            user, (int)itemIdx, a4, c1, c2, str, accId, a10);
        return;
    }

    // P52：receiver 链保持单语句，ORIG 把 c2/c1/itemIdx 先物化 edi/esi/ebx 跨 getter 保活。
    int slot = user->getCurCharacInvenW()->AddAvatarItem(
        (int)itemIdx, a4, false, c1, c2, str, (eAvatarItemAddReason)0, false, 0);
    if (slot < 0)
    {
        const char* msg = g_scriptStringManager_.findString(4, "game_server_msg_104", (bool*)0);
        len = (int)strlen(msg);
        // P0：characNo 无槽往返 ⇒ 原码是实参内联（ORIG 无 -0xc 槽）。
        WongWork::CMailBoxHelper::ReqDBSendNewAvatarMailCashShop(
            user, user->getCurCharacNo(), (int)itemIdx, a4, c1, c2, str, msg, len);
    }
    else
    {
        user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)1, slot);
    }
}