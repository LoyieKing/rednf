int Dispatcher_SecurityCardAuthReq::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0820a2c4
    // ORIG cmp $0x2,get_state; setle → state<=2 走断言
    if (user->get_state() <= 2)
    {
        return LineFunc(0xa5fd,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    // card.isActivate() || pad.isActivate()
    // ORIG 0x0820a316..0x0820a344：短路 `||` 物化为 `jne L_true / je L_false /
    // mov $1 / jmp / mov $0 / test %al,%al; je <尾部失败块>`，且汇聚值**无栈槽往返**
    // （P0）⇒ 具名 bool 必顶槽（帧 0x18→0x28）。改 P226 扩展：语句表达式包住整个条件
    // （内无具名变量）+ 正极性包裹，失败块归函数尾（P214① 负极性恒 je/jne 反演）。
    if (({ user->getSecurityCard()->isActivate() ||
           user->getPad()->isActivate(); }))
    {
        if (user->getSecurityCard()->isActivate())
        {
            // T1/T2 直证：ORIG 0x0820a36c `cmp $0x2,%eax; seta` ⇒ 无符号比较。
            if ((unsigned int)user->getSecurityCard()->getFailCnt() > 2)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa6, 0x7a);
                return 0;
            }
        }
        else
        {
            if (user->getPad()->getFailCnt() > 2)        // ORIG cmp $0x2; setg
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa6, 0x88);
                return 0;
            }
        }

        // ORIG：Check 返回值丢弃，恒 return 0
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0);
        return 0;
    }
    return LineFunc(0xa625,
                    __PRETTY_FUNCTION__,
                    0, 0);
