// [round5-A20 复核·N121] fresh .o 102/101：判据⑪ 直方图逐值全等 {−0x9,−0x10,−0x14}；
//   唯一残差=我方 switch 尾多 1 条 `jmp`（.o 内 1272: nop; 1273: jmp 1276; 1275: nop
//   —— default 岛），ORIG 单 nop 直落 epilogue ⇒ 4.4 自身 epilogue-threading 尾岛
//   （N121，四形实测不可消）。
// [round5-A20 D08 试前闸门] **我方岛数 − ORIG 岛数 = 1**（< 闸门阈值 2）⇒ 按
//   C05③ 闸门归「4.4 rel8 跳板/epilogue-threading」族（同 A28/A06 反例），
//   不做多守卫合并试验（无 ≥2 岛差即无「守卫相邻同目标」可赚）。
void Inter_LoadEtc::verifyCharacVillage(CUser* user)
{
    // [b28d-solo-Inter_LoadEtc_impl] verdict=N 102/101（fresh）：vill int 槽（movsbl→-0x14）、
    // needMove 预清+条件置位、嵌套 if（P225 推广）均已归位；残余为 switch 尾 default 岛
    // （4.4 恒发 `jmp;nop`，P223 边界=4.4 自身 epilogue-threading，四形实测不可消）。
    // [Lane17 I 佐证] 判据⑪ 直方图**逐值全等** {−0x9,−0x10,−0x14} ⇒ 确认非槽问题；
    // 归一化后仅剩尾岛（我方 `1272: nop; jmp 1276` vs ORIG 单 `nop`）与跳转表
    // `mov 0xf4(%eax)`（.o 未重定位假象，链接后 = ORIG `mov 0x8c8a480(%eax)`）⇒ N121 封顶。
    if (G_GameWorld()->IsPVPChannel())
        return;

    // [b28d-solo-Inter_LoadEtc_impl] ORIG 0x084e8693 `movsbl %al,%eax; mov %eax,-0x14`
    // ⇒ vill 为 int 槽（非 char）。
    int vill = (int)(char)user->getCurCharacVill();
    Village* v = G_GameWorld()->GetVillage(vill);
    if (v == 0)
    {
        user->setCurCharacVill(1);
        return;
    }

    // ORIG 0x084e86ce：`movb $0x0,-0x9` 预清 + 条件置位（P198①/P194）
    char needMove = 0;
    if (user->get_charac_level() < v->m_field2c)   // +0x2c（最低等级门槛）
        needMove = 1;
    // [b28d-solo-Inter_LoadEtc_impl] P225 推广：ORIG 两守卫各自 `je` 直跳同一落点
    // ⇒ 嵌套 if（`&&` 单表达式会被 4.4 物化 bool 岛 +7 条）。
    if (v->m_questIdx != 0)                                    // +0x30
    {
        if (!user->getCurCharacQuestR()->isClearQuest(v->m_questIdx))
            needMove = 1;
    }

    if (needMove == 0)
        return;

    switch (vill)
    {
    // [PoolF 2026-09-11 复验] `break`→`return` 七处（消除 switch 出口跳板）实测：
    // 读数逐值不变（102/104，ndiff 1 块 = 我方尾 `jmp <T>` 一条），4.4 恒发
    // `jmp <nop 岛>; jmp epilogue` 跳板 ⇒ N121 封顶已复证，维持 break 形。
    case 2:  user->setCurCharacVill(1); break;
    case 3:  user->setCurCharacVill(2); break;
    case 4:  user->setCurCharacVill(1); break;
    case 5:  user->setCurCharacVill(2); break;
    case 6:  user->setCurCharacVill(3); break;
    case 9:  user->setCurCharacVill(3); break;
    case 11: user->setCurCharacVill(2); break;
    }
}