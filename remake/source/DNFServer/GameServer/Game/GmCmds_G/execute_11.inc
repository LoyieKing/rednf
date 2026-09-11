// 万次蒙特卡洛统计强化券成功率并向 GM 回播聊天消息。
// [b28d-19 / b28c-R26 复验] verdict=DIFF(routed) [N126 + 待解 memset-edx] ORIG 0x084b1b96
//   orig/ours=233/229（gap +4）。本轮改动：
//   ① 删具名 itemIdx 局部（P0/P183）：ORIG 0x084b1bba `mov 0x8(%eax),%ebx` 把
//      m_slot 装 callee-saved 直传 find_item；具名局部会多占 -0x34 槽。
//   ② 死缓冲声明序改为 dead1e → dead400（ORIG 物理序 0x084b1bf4(0x400) →
//      0x084b1c3e(memset) → 0x084b1c5d(0x1e) → 0x084b1caf(0x400)）。
//   ③ 试 `memset((char*)p,0,N)`（P68）复现 ORIG 0x084b1c3e `lea→%eax;
//      mov $0x17318,%edx; mov %edx,0x8(%esp)` 的 edx 中转形 —— 实测无效
//      （4.4 仍 `movl $imm,0x8(%esp)`）⇒ 并入「待解：memset 首参 edx 中转」
//      （与 CAchievementReport::execute 同现象）封顶。
// [round5-GmCmds_G 复核] 旧 N126「整帧平移 8 不可消」判词被**部分推翻**：
//   ④ §6.5.1 编码修复（语义轴）：sprintf 格式串 ORIG 0x08c8035c 实字节为
//      EUC-KR 50 字节，原源码误写 UTF-8 63 字节（ORIG 无此串）⇒ 已改字节转义。
//   ⑤ P0 双向律：ORIG 0x084b1d41 `mov -0x38(%ebp),%eax; mov %eax,-0x28(%ebp)`
//      后三次 stackable 调用点全 `mov -0x28(%ebp),%eax` ⇒ 原码有具名
//      `CStackableItem* stackable`（-0x28 槽）；补上后 229→231 条。
//   ⑥ 类型轴：Amplify 分支局部 ORIG 0x084b1d9d `movsbl %al,%eax; mov %eax,-0x20`
//      + `cmpl $0x0/$0x64,-0x20` 直证 4 字节 int（Reinforce 分支仍 1 字节 char，
//      ORIG `mov %al,-0x21` + `cmpb`）⇒ 改 int 后 231→232 条。
//   残余 = ① N98 槽序（4.4 按首次使用序：我方 stackable 落 -0x2c，深于
//   success/failure(-0x28/-0x24)；ORIG 4.1 按声明序把它排在二者之后）；
//   ② 首槽常数收窄到 4 字节（user -0x38 vs ORIG -0x3c，原为 8）；
//   ③ memset 第 3 实参 edx 中转 1 条（ORIG 0x084b1c44 `mov $0x17318,%edx;
//   mov %edx,0x8(%esp)`，4.1 独有；P68 `memset((char*)p,0,N)` 两形实测无效）
//   ⇒ 三项均源级不可翻，orig/ours=233/232（gap +1），p167 CLEAR。
void CUpgradeCardDebug::execute()
{
    CUser* user = GetUser();
    if (user == 0)
        return;

    // P0/P183：ORIG 0x084b1bba `mov 0x8(%ebp),%eax; mov 0x8(%eax),%ebx` 把 m_slot
    // 装 callee-saved %ebx 直传 find_item（无具名槽）⇒ 内联表达式，不设具名局部。
    CItem* item = G_CDataManager()->find_item(
        reinterpret_cast<const IntSlotView*>(this)->m_slot);
    if (item == 0)
        return;

    // ORIG：call is_stackable 后 xor $0x1/test 判 _Bool 假值即出函数。
    if (item->is_stackable() == false)
        return;

    // ORIG 入口对三块栈缓冲整块清零（0x400/0x1e/0x400 内联展开 +
    // 95000 大缓冲 memset call）；除 sprintf 目标外均为死存储。
    // ORIG 物理顺序 0x084b1bf4(0x400) → 0x084b1c3e(memset) → 0x084b1c5d(0x1e)
    // → 0x084b1caf(0x400) ⇒ dead400 必须声明在 dead1e 之后。
    char msg[1024] = { 0 };
    unsigned char deadLarge[95000];
    // P68：ORIG 0x084b1c3e 先 `lea` 落 %eax、size 经 %edx 中转再压栈
    // （`memset((char*)p,0,N)` 形）；裸 memset(p,0,N) 直接 movl 立即数。
    memset((char*)deadLarge, 0, sizeof(deadLarge));
    unsigned char dead1e[30] = { 0 };
    unsigned short dead400[512] = { 0 };

    int rate = 0;                                   // -0x34
    switch (item->get_index())
    {
    case 0x1d29:
    case 0x1c69:
    case 0x1c6a:
    case 0x1c6b:
    case 0x1e3f:
    case 0x1e40:
    case 0x1e86:
        rate = 100;
        break;
    default:
        break;
    }

    // P0 双向律：ORIG 0x084b1d41 `mov -0x38(%ebp),%eax; mov %eax,-0x28(%ebp)`
    // （switch 之后、首次 stackable 调用之前）起，后续三次调用点全部
    // `mov -0x28(%ebp),%eax` ⇒ 原码具名 CStackableItem* 局部（-0x28 槽），
    // 非内联 static_cast 右值。
    CStackableItem* stackable = static_cast<CStackableItem*>(item);

    if (stackable->IsReinforceTicket())
    {
        signed char value = (signed char)stackable->GetReinforceRate();
        if (value < 0 || value > 100)
            rate = 0;
        else
            rate = (int)value;
    }
    else if (stackable->IsAmplifyReinforceTicket())
    {
        // 类型轴直证：ORIG 0x084b1d9d `movsbl %al,%eax; mov %eax,-0x20(%ebp)` +
        // 0x084b1da3/0x084b1da9 `cmpl $0x0/$0x64,-0x20(%ebp)` ⇒ 本分支局部是
        // 4 字节 int（非 Reinforce 分支的 1 字节 char，后者 ORIG 用 cmpb）。
        int value = (signed char)stackable->GetAmplifyReinforceRate();
        if (value < 0 || value > 100)
            rate = 0;
        else
            rate = (int)value;
    }

    int success = 0;                                // -0x30
    int failure = 0;                                // -0x2c
    for (int i = 0; i <= 9999; ++i)                 // -0x1c
    {
        if (get_rand_int(100) < rate)
            ++success;
        else
            ++failure;
    }

    // [round5-GmCmds_G 语义修复 · §6.5.1 编码盲区] ORIG 0x08c8035c 起的实字节是
    // EUC-KR（`\xbe\xc6\xc0\xcc\xc5\xdb:%s(%d) \xbc\xba\xb0\xf8\xc8\xae\xb7\xfc:%d`
    // `  (%d\xb9\xf8 \xbc\xba\xb0\xf8, %d\xb9\xf8 \xbd\xc7\xc6\xd0)\n`，共 50 字节，
    // 已逐字节命中 ORIG），而原源码误写 UTF-8（63 字节，ORIG 无此串）⇒ 运行期
    // GM 聊天回显乱码。改字节转义使发射字节 == ORIG；源码文件保持 UTF-8。
    sprintf(msg,
            "\xbe\xc6\xc0\xcc\xc5\xdb:%s(%d) \xbc\xba\xb0\xf8\xc8\xae\xb7\xfc:%d"
            "  (%d\xb9\xf8 \xbc\xba\xb0\xf8, %d\xb9\xf8 \xbd\xc7\xc6\xd0)\n",
            item->GetItemName(), item->get_index(), rate, success, failure);
    SendChatMsg(GetUser(), msg);
}