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
    char login[0x20e];
    char reply[0x20e];
    memset(login, 0, sizeof(login));
    // b28d-lane5：ORIG 0x0868xxx 每次字段访问重发 `lea -0x226(%ebp)`（P183 地址
    // 重物化铁律）⇒ 删除 lp 指针缓存，全表达式展开。
