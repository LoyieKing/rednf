// ORIG T 0x0868152c sz0x1e0：PC 房 login 通知。填 stLogin（login[0]=3/acc 名/
// charac 名/ip/uid/accid）→ stLoginPacket 打包 → SendTcpPacket；随后
// ReadPacket 读回包（ORIG 以 pkt 自身为 this、reply 为 packet 源）。
// b28d-lane5 N封顶: P11 反向符号扩展; N82/P112 槽号分配 | ORIG 0x0868152c | _ZN5CUser23SendLoginToPCRoomServerEv
// [H3CUser 2026-09-11 · Main 裁决] `nexon_packet.h` 的 `stLogin::m_accLen/m_charLen`
//   → `unsigned char` 的**头改动弃用**：本函数已用 `(unsigned char)` cast 等效复现
//   ORIG 0x08681611..0x08681636 的 `movzbl %al,%reg`（见函数体内注释），cast 形已达成
//   ORIG 形，改头收益为零且多一个跨 TU 风险面 ⇒ 维持现状、不再报该项。
void CUser::SendLoginToPCRoomServer()
{
    // 2026-09-10 gA（A 组）：**实现体不在本 .inc**（本文件仅函数头 + login/reply
    //   声明 + memset(login)），主体在聚合器 CUser.cpp:3306-3330 ⇒ 本组写域外，未动。
    //   已定位的可落地差异（供 .cpp 属主/Main 使用；ORIG 0x0868152c，我方 136 vs 113）：
    //   ① `LP_LOGIN->m_type = 3` ORIG 0x08681556 为 `movb $0x3,-0x226(%ebp)`（数组直址
    //      写），我方生成 `lea -0x22e(%ebp),%eax; movb $0x3,(%eax)`；
    //   ② 具名 `const int ip = get_public_ip_address();` 多余——ORIG 0x086815dc
    //      `mov %eax,-0x25(%ebp)` 直接落 stLogin+0x201（我方才经 -0x20 槽再转写）；
    //   ③ printf 实参 ORIG 全为 `movzbl %al,%reg`（0x08681611..0x08681636）入
    //      callee-saved，我方多 `movsbl %al,%eax` + 栈槽（-0x144c/-0x1450/-0x1454）
    //      ⇒ 去掉 `(int)` cast 改传 `(unsigned char)`；
    //   ④ 帧 0x147c vs 0x146c、reply 起点 -0x43c(我方) vs -0x434(ORIG) 由 ①②③ 连带。
    // 2026-09-11（G2 类型轴）：ORIG 0x08681556 `movb $0x3,-0x226(%ebp)`、
    //   0x086815dc `mov %eax,-0x25(%ebp)`、0x086815e3/0x086815f1 `mov %eax,-0x20/-0x1c(%ebp)`
    //   全部是 **ebp 常量偏移直址**（无 `lea base,%R` 前置）⇒ 原码缓冲就是 stLogin 对象，
    //   不是 `char[0x20e]` + cast。改对象后我方 19 条 `lea -0x22e(%ebp),…` 消失，
    //   帧 0x147c→0x146c 由 ip 槽收敛补齐。stLogin 为 #pragma pack(1)、sizeof==0x20e
    //   （nexon_packet.h:27），与数组布局逐字节一致。
    nexon_packet::stLogin login;
    nexon_packet::stLogin reply;
    memset(&login, 0, sizeof(login));
    // b28d-lane5：ORIG 0x0868xxx 每次字段访问重发 `lea -0x226(%ebp)`（P183 地址
    // 重物化铁律）⇒ 删除 lp 指针缓存，全表达式展开。
    login.m_type = 3;
    strcpy(login.m_account, get_acc_name());
    login.m_accLen = (char)strlen(get_acc_name());
    strcpy(login.m_characName, getCurCharacName());
    login.m_charLen = (char)strlen(getCurCharacName());
    // ORIG 0x086815dc：`call get_public_ip_address; mov %eax,-0x25(%ebp)` 直落 stLogin+0x201
    // ⇒ 删具名 `const int ip`（我方原多 store/load 各 1 条 + lea 1 条）。
    login.m_ip = (unsigned int)get_public_ip_address();
    login.m_canTry = 0;
    login.m_userIdx = (unsigned int)GetUID();   // ORIG：直接写 stLogin+0x206（-0x20(%ebp)）
    login.m_accId = (unsigned int)get_acc_id(); // ORIG：直接写 stLogin+0x20a（-0x1c(%ebp)）
    printf(
        "SEND nexon_packet::stLogin login:%d AccLen:%d Acc:%s CharLen:%d Char:%d "
        // [L9·2026-09-11 §6.5 串盲区] ORIG 0x0868167d `mov [esp],0x8ceff8c` ⇒ 格式串 VA 0x8ceff8c
        //   = 文件偏移 0xca7f8c，即本串起始；.rodata 原字节以 `m_id:%d\x00` 结束（**无尾 \n**）。
        //   旧形多一个换行 ⇒ 与 ORIG 日志输出不一致（AE 盲区）。
        "Addr:%d canTry_:%d UserIdx:%d m_id:%d",
        // ORIG 0x08681611..0x08681636：char 字段实参一律 `movzbl %al,%reg`（零扩展）
        // ⇒ 加 (unsigned char) cast（我方原为 `movsbl %al,%eax` + 3 个中转栈槽）。
        (unsigned char)login.m_type, (unsigned char)login.m_accLen, login.m_account,
        (unsigned char)login.m_charLen, (int)login.m_characName,
        (int)login.m_ip, (unsigned char)login.m_canTry, (int)login.m_userIdx,
        (int)login.m_accId);

    nexon_packet::stLoginPacket pkt(login);
    // b28d-GrpD（P0/P183）：ORIG 0x086816a1 `call GetLength` → 0x086816c6 前
    //   `lea -0x1438(%ebp),%ecx; mov s_pcroom_proxy,%edx; mov %eax,0x8(%esp)`
    //   —— GetLength 返回值**无栈槽**直装载；我方 `int len` 具名局部多
    //   `mov %eax,-0x1c(%ebp)` + `mov -0x1c(%ebp),%ecx` 各 1 条 + 4B 帧
    //   （探针 /tmp/L9/D/p2.cpp 形 A vs 形 B 直证）⇒ 删局部内联实参。
    GlobalData::s_pcroom_proxy->SendTcpPacket((char*)&pkt, pkt.GetLength());
    memset(&reply, 0, sizeof(reply));
    pkt.ReadPacket((const char*)&reply, reply);
}