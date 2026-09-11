// ORIG 0x081cc106（报告 Dispatcher_UpgradeItemSeparate/process.md）：
// err>0 → SendCmdErrorPacket((ENUM_CMDPACKET)0x1b7, err&0xff) 后 return -1；
// 否则 m_itemUpgradeSeparate(+0x24)->item_upgrade_system(user, msg, 0x1b7)。
// [b28c-33 判词 N] ORIG 081cc152 `mov 0x14(%ebp),%eax; mov %eax,-0x10(%ebp)` 读第 4
// 形参槽（3 参函数越界读调用者栈）并落死局部 -0x10，合法 C++ 不可复现；残 2 条即此。
// 我方 36/ORIG 38，其余调用序列与槽全等（p167 CLEAR）。
int Dispatcher_UpgradeItemSeparate::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int err = check_error(user, msg);
    if (err > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b7, (unsigned char)err);
        return -1;
    }
    // b28d-D0: ORIG 0x081cc152 `mov 0x14(%ebp),%eax; mov %eax,-0x10` = param 死槽
    //   （P198 声明+赋值两句保地址 store）；0x081cc158 `mov 0x10(%ebp),%eax;
    //   mov %eax,-0xc` = msg 具名指针槽（4.4 直传 &msg 会折叠该槽）。
    ParamBase* dp;
    dp = &param;
    (void)dp;
    MSG_ITEM_UPGRADE* pm;
    pm = reinterpret_cast<MSG_ITEM_UPGRADE*>(&msg);
    // ORIG 0x081cc15e 链式直取 m_itemUpgradeSeparate（无 system 具名槽）。
    ((WongWork::IItemUpgrade*)G_CDataManager()->m_itemUpgradeSeparate)
        ->item_upgrade_system(user, pm, 0x1b7);
    return 0;
}