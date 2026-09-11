// Secu_DataControl::UpdateSecurityServiceState —— ORIG T 0x08286e40
// [0x08286e40,0x08287081]。service==0：SecurityCard 已激活→init；
// service==1：CPad 已激活→若已认证则 flag=1，否则将 msg 截 8 字节 AES 加密后
// setFailCnt(0)+setPassword；任一分支生效则发送 0x159 通知包（service/flag）。
// [b28d-18 N78] EXTRA memset×2：4.1 内联清零 vs 4.4 call（N78） | ORIG 0x08286e40
// [b28d-solo-GlobalData] DIFF 封顶：p167 EXTRA memset×2 = N78（4.1 内联清零/4.4 call）；
// MISSING 空 ⇒ 语义无缺口。
// [G7-sub 2026-09-11 判词] Secu_DataControl::UpdateSecurityServiceState（ORIG T 0x08286e40）：
// 私有 .o 实测 ORIG 151 / 我方 153（gap 2，我方多）。逐条对照残差**仅两处**：
//   · 我方 145 位多 `jmp <epilogue>; nop` 尾岛（ORIG 0x08270777 自 PacketGuard D1 call
//     直落 epilogue）⇒ 已知 N121（4.4 epilogue-threading），否定早返/共享尾标签/
//     显式 return 三形（P285 反向）。
//   · 数据符/字符串归一后其余全等。p167 调用点差集 CLEAR（25/25，仅别名）。
// 结论：**封顶，无改动**（语义完成，byte 层 N）。
void Secu_DataControl::UpdateSecurityServiceState(CUser* user, unsigned char service,
                                                  unsigned char flag, char* msg)
{
    if (user == 0)
    {
        return;
    }
    bool done = false;
    // ORIG 0x08286e6c..0x08286e7c：`movzbl service; test %eax,%eax; je L0;
    // cmp $0x1,%eax; je L1; jmp end`（形参载入 + 寄存器比较链）⇒ 源形是
    // `switch (service)`（P251/P286；if/else-if 在 4.4 恒折 cmpb 内存直比）。
    switch (service)
    {
    case 0:
        // ORIG 0x08286e81..0x08286eaf：getSecurityCard() 结果直作 receiver，
        // 全程无栈槽往返（P0/§4.0 槽-局部双向对应律）⇒ 禁具名指针局部。
        if (user->getSecurityCard()->isActivate())
        {
            user->getSecurityCard()->init();
            done = true;
        }
        break;
    case 1:
        if (user->getPad()->isActivate())
        {
            // ORIG 0x08286ee6..0x08286fcb：`call isCertified; xor $1; test %al,%al;
            // je <flag=1 块>` ⇒ 源形是否定式 `if (!isCertified())` 且 flag=1 走
            // else 块置于函数尾（P168a②/P292 判据）。
            if (!user->getPad()->isCertified())
            {
                // ORIG 0x08286ef1..0x08286f15：两段常量内联 rep stos（9 dword
                // = 36B）⇒ 源形 `char buf[36] = {0}`（P214③/P312/P328⑨），
                // 写 memset 在 4.4 退化成 call（N78 可翻形）。
                char plain[36] = {0};
                char enc[36] = {0};
                strncpy(plain, msg, 8);
                cMyTrace trace(
                    __PRETTY_FUNCTION__,
                    0xa0, 0);
                trace("[Secu_Web_Conn] (%s)  ", msg);
                encryptAES_(GlobalData::s_pcryptRijndael_MousePassword_, plain, enc);
                user->getPad()->setFailCnt(0);
                user->getPad()->setPassword(enc);
                done = true;
            }
            else
            {
                flag = 1;
            }
        }
        break;
    }
    // [K8GlobalData 2026-09-11 · 负结论，勿重烧] 残差 1 条 jmp（我方 151 vs
    // ORIG 150，ORIG-only=- / OURS-only={'jmp':1}）实测**不可翻**：把
    // `if (done) {…}` 改负极性早退 `if (!done) return;`（body 直落 epilogue）
    // ⇒ 实测 154→**158**（反劣：早退点自身需 jmp，且 EH 清理块外提）。
    // 维持原形，余 1 条记 N121（4.4 epilogue-threading）。
    if (done)
    {
        PacketGuard guard;
        guard.put_header(0, 0x159);
        guard.put_byte(service);
        guard.put_byte(flag);
        guard.finalize(true);
        user->Send(guard);
    }
}