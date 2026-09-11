// ============================================================================
// handleDieUser（ORIG 0x081494a4）
// [b28d-46 判词·N/P197④+P168a③b+N82] 168 vs 172，p167 CLEAR（29/29）。
// for(i=0;i<=3;++i)+continue 形复现双初始化（P191）与尾部 setle 物化（P51）。
// [b21 N·P197-④] 语义已闭合：找成员循环 `checkValidUser(i) && get_user(i)==user`
//   的取反判定——ORIG(4.1) 发 `sete %al; test; je`，4.4-O0 对 call 结果直比恒发
//   `mov $1/mov $0 + test/jne`（P197-④：源形不可复现，禁改内联比较=双调用真 bug）；
//   循环头 `movl $0,-0xc` 双写为 4.1 代码生 quirk（int i=0）。返回路径 ORIG 在各
//   return 点直发 `mov $0x0/%1,%eax`+共享尾，4.4 以 %ebx 合并 epilogue（N82/寄存器分配）。
//   while(true){if(3<i)return 1;...break;++i;} 形态 + 时间比较改 `GetRevivalTime() >
//   getCurSec()` 已对齐 4.1 求值序/setg 方向；逐调用逐分支语义逐一相等，封 N。
// [L5Free49to60 2026-09-11 复核] fresh 168 vs 170（gap −2，与基线一致）。bcmp 逐条：
// ①第 1 守卫 ORIG `call; test %al; sete %al; test %al; je` vs 4.4 `xor $1`（N122/
//   P168a④：4.4 对布尔取反恒 xor、4.1 的 sete 内存物化不可复现）；
// ②packet2 段 4.4 多 5 条 `mov/lea/call/jmp` 中继（P63 早退共享出口族）。
// 均属跨版本结构性差异 ⇒ 维持封 N。
// ============================================================================
bool CBossPlay::handleDieUser(CUser* user)
{
    // [b28d-46 实测] ORIG 双 `movl $0,-0xc`（P191）+ 尾部 `cmpl $3; setle; test;
    // jne`（P51）+ 两独立守卫各 `je INC`（P133）⇒ for + continue 形。
    int i = 0;
    for (i = 0; i <= 3; ++i)
    {
        if (!m_party->checkValidUser(i))
            continue;
        if (!(m_party->get_user(i) == user))
            continue;
        if (isMemberAlive(i))
        {
        setMemberAlive(i, false);

        PacketGuard packet1;
        packet1.put_header(0, 0x20);
        packet1.put_short(user->get_unique_id() & 0xffff);
        packet1.put_byte(0);
        packet1.finalize(true);
        m_party->send_to_party(packet1);

        if (user->GetRevivalTime() > GlobalData::s_systemTime_.getCurSec())
        {
            user->SetRevivalTime(0);
            user->SetUseAPCPotionTime(0);
            setMemberAlive(i, true);

            PacketGuard packet2;
            packet2.put_header(0, 0x20);
            packet2.put_short(user->get_unique_id() & 0xffff);
            packet2.put_byte(2);
            packet2.finalize(true);
            m_party->send_to_party(packet2);
        }
        return 1;
        }
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x2b, 0x12);
        return 0;
    }
    return 1;
}
