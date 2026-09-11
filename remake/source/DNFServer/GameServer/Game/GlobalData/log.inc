// ---- PcRoom::log —— T 0x082707c0（同名报告全文转写）------------------------
// [b28d-18 N7] p167 CLEAR；残差=callee-saved 角色轮换 + 槽位（N7/N82） | ORIG 0x082707c0
// [b28d-solo-GlobalData] NEAR 封顶 N7/N82：p167 CLEAR；残差=callee-saved 角色
// 轮换（esi/ebx/edi 循环移位）+ 槽深差 4（-0x19/-0x1d、-0x2c/-0x1c，P230 定稿
// 4.4.7 分配固定不可择）+ 符号地址重定位显示伪差。
void RentalMachinePcRoom::log(CUser& user, int itemId)
{
    // [DataLane P212/P339] ORIG 槽（深→浅）= serverEnv(-0x2c) / CMyFileLog(-0x24) /
    // serverInfo(-0x19)：serverEnv 占**最深**槽 ⇒ 原码为函数头前置声明（4.4 头部
    // 标量按「先声明 → 更深槽」分配）；旧块内声明使其落 CMyFileLog 之上(-0x1c)。
    // [DataLane P232] ORIG `sub $0x4c` vs 我方 `sub $0x3c`：0x10 帧差 + 判词已录
    // 的 ORIG 空槽 -0x28 ⇒ 补 0x10 字节 dead 数组（4.4 -O0 对未读写局部照样占帧）。
    // [DataLane 第 2 试] 交换 framePad/serverEnv 声明序：4.4 头部分配下 pad 占更深
    // 槽、serverEnv 落 ORIG 的 -0x2c（ORIG serverEnv 深于 CMyFileLog -0x24）。
    char framePad[0x10];
    (void)framePad;
    int serverEnv;
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
    serverEnv = (int)serverInfo;
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

    CMyFileLog flog(__PRETTY_FUNCTION__, 0x12e);
    flog(pData,
         "[RENT] m_id(%d) char_no(%d) server_info(%d) item_id(%d)", accId,
         charNo, serverEnv, itemId);
}