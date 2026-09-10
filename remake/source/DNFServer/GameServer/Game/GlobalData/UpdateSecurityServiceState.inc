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
    if (done)
    {
        PacketGuard guard;
        guard.put_header(0, 0x159);
        guard.put_byte(service);
        guard.put_byte(flag);
        guard.finalize(true);
        user->Send(guard);
    }
