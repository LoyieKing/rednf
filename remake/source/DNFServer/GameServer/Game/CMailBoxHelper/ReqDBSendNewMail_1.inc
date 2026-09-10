int CMailBoxHelper::ReqDBSendNewMail(CUser* user, MSG_MAILBOX_SEND* msg,
                                     unsigned int nCharacNo, bool bA,
                                     unsigned int nB, int nC)
{
    // ORIG 0x08555b14：发信核心（金币/物品/AVATAR/CREATURE 校验、扣费、DB 请求）。
    // MSG_MAILBOX_SEND 为 opaque（PacketTypes.h m_pad[0x100]），按 ORIG 偏移访问：
    //   +0x11 收件角色名, +0x2e 金币额, +0x36 物品类型选择, +0x37 槽位,
    //   +0x39 item id, +0x3d add_info/目标角色, +0x41 标题长度, +0x45 标题。
    // 返回码：0=成功 1=物品不存在 0x10=金币不足 0x11=校验失败 0x17=类型/绑定不符。
    // [b28d-solo-CMailBoxHelper] P0/P183：ORIG 无 `m` 具名局部（每处直读形参
    // 槽 `mov 0xc(%ebp),%eax`，0x08555b28/0x08555b43/…）⇒ 删除局部改宏直读。
    // [b28d-solo-CMailBoxHelper] P190/P31 探针：ORIG 槽深 4（tax -0x28 vs
    // -0x24）；double framePad 实测撑帧 +8 过头，改 int dead 局部。
    int framePad;
    (void)framePad;
