void ShowLogo()
{
    std::cout << std::endl;
    std::puts("**********************************************************");
    std::puts("* +---   +-+ +  +----         DUNGEON & FIGHTER          *");
    std::puts("* |   |  | | |  |                                        *");
    std::puts("* |   |  | | |  +----                                    *");
    std::puts("* |   |  | | |  |      Open Beta Test on 2005.08.10~     *");
    std::puts("* +---   + +-+  |      Copyright(c) 2004,2005 Neople Co. *");
    std::puts("**********************************************************");

    CommonTime time;                          // 栈上对象（ebp-0x18，布局同 ORIG：+0..+4 年/月/日/时/分）
    time.SetCurTime();
    // ORIG 依次读 ebp-0x18/-0x17/-0x16/-0x15/-0x14 五字节（movsbl）→ 年/月/日/时/分
    // [b28c-lane24] 不缓存 `const char* p`（P183 地址重物化铁律）：具名指针使
    // 4.4 生 `lea -0x18; mov %eax,-0xc; mov -0xc,%eax; add $K` 链，ORIG 直接
    // `movzbl -0x18(%ebp),%eax` 栈相对 disp ⇒ 每次读取内联全表达式。
    // 具名标量视角（CUserCharacInfo.h CommonTime 匿名 union）：ORIG 单条
    // `movzbl -0x18..-0x14(%ebp),%eax` disp 读，禁数组成员索引（lea+add）。
    std::printf("[!] Service Date (%02d-%02d-%02d/%02d:%02d)\n",
                (int)time.m_year, (int)time.m_month, (int)time.m_day,
                (int)time.m_hour, (int)time.m_min);
