int Arad_INTER_kUseAvatarRecharge::dispatch_sig(CUser* user, char* body, int len)
{
    // [b28d-59 P0 清绑] ORIG 0x08199fxx：各失败路径 `mov $imm,%ebx; jmp end`＝
    // 出口寄存器跨 call 存活；原 `register int ret asm("ebx")` 绑定已按 P0/N96
    // 清为纯 register 存储类提示（P231：裸 register 合法且仍参与分配决策）。
    register int ret;
    if (user == 0)
    {
        ret = 0x68e;
        goto out;
    }
    if (user->getCurCharacR() == 0)
    {
        ret = 0x691;
        goto out;
    }

    {
        arad::SigAradUseAvatarRecharge* sig = (arad::SigAradUseAvatarRecharge*)body;   // ORIG 读 0x10(%ebp)=body
        if (sig == 0)
        {
            LogManager::logFormat(1, "localjapan/Arad_InterDispatcher.cpp", __PRETTY_FUNCTION__,
                                  0x696, "AVATAR USE AVATAR RECHARGE INTER_DSP(SigAradUseAvatarRecharge) null.");
            ret = 0x697;
            goto out;
        }

        // P48 + ORIG 寄存器出口：! 语义产生 xor $0x1 翻转；Check/Recharge 每次各自 Get()。
        if (!Singleton<AvatarRechargeServer>::Get()->CheckConditionUseRecharge(user, sig))
        {
            ret = 0;
            goto out;
        }
        if (!Singleton<AvatarRechargeServer>::Get()->Recharge(user, sig))
        {
            ret = 0;
            goto out;
        }
        {
            PacketGuard packet;
            packet.clear();
            packet.put_header(1, 0x24d);
            packet.put_byte(1);
            packet.put_int((int)sig->enduranceAfter);       // +0x0c（操作后耐久）
            packet.put_int(sig->addEndurance);              // +0x04（本次增加耐久）
            packet.put_short((unsigned int)sig->avatarSlot);  // +0x00（avatar 槽）
            packet.finalize(true);
            user->Send(packet);
            ret = 0;
            goto out;
        }
    }
out:
    return ret;
