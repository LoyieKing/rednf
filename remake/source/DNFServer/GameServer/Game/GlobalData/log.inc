void RentalMachinePcRoom::log(CUser& user, int itemId)
{
    char serverInfo = 0;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        serverInfo = (char)user.GetServerGroup();
    }
    else
    {
        serverInfo = (char)G_CEnvironment()->m_serverEnvir.m_serverGroup;
    }
    // [b28d-solo-GlobalData] ORIG 0x082707e8 `movsbl -0x19; mov %eax,-0x2c` 槽往返
    // ⇒ 原码有具名 int 提升局部（P0 有槽必有局部）。
    const int serverEnv = (int)serverInfo;
    // [b28d-solo-GlobalData] P231/P248：ORIG 0x082707c0 三个值跨 CMyFileLog ctor
    // 保活于 callee-saved（esi=charNo/ebx=accId/edi=m_data）⇒ 纯 register 提示
    // （非 asm 绑定，P0 合规）；缺 register 时 4.4 落栈槽（-0x10/-0xc）。
    // [Lane7GlobalData · P248补四 声明序旋钮——**已实测回滚**] pData 提首位后
    // register 角色未转正，反把本件由 NEAR 打成 DIFF（TU：near 11→9、diff 38→40）
    // ⇒ 恢复原声明序（charNo/accId/pData）。判词：4.4 的 callee-saved 归属不随
    // 声明序轮转（与 P248补四 的「位置」结论在**多 register 局部**场景不适用）。
    register const int charNo = user.get_charac_no(-1);
    register const unsigned int accId = user.get_acc_id();
    register char* const pData = m_impl->m_data;

    CMyFileLog flog("void RentalMachinePcRoom::log(CUser&, int)", 0x12e);
    flog(pData,
         "[RENT] m_id(%d) char_no(%d) server_info(%d) item_id(%d)", accId,
         charNo, serverEnv, itemId);
