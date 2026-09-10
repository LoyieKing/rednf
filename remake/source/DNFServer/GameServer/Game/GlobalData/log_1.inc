void RentalMachineDimension::log(CUser& user, int itemId)
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
    // [b24ab] 显式 int 提升到命名局部（ORIG +0x3c movsbl → -0x2c 槽）
    const int serverEnv = (int)serverInfo;
    // [b24ab] ORIG 将 charNo/accId/m_data 缓存在 callee-saved（esi/ebx/edi）跨
    // CMyFileLog 构造存活 ⇒ register 强占（P197②形）
    // [Lane7GlobalData · P248补四 声明序旋钮——**已实测回滚**] 同 PcRoom 版：
    // 提前 pData 反使本件 NEAR→DIFF ⇒ 恢复原声明序。
    register const int charNo = user.get_charac_no(-1);
    register const unsigned int accId = user.get_acc_id();
    register char* const pData = m_impl->m_data;

    CMyFileLog flog("void RentalMachineDimension::log(CUser&, int)", 0x71);
    // [G7-sub] 结论（N115/N126/N82 封顶）：ORIG 58 / 我方 58。ORIG 0x0826ef1c
    //   `sub $0x4c,%esp` + 槽 serverInfo(-0x19)/CMyFileLog(-0x24)/serverEnv(-0x2c)
    //   （另有空槽 -0x28）；我方 `sub $0x3c,%esp` + -0x1d/-0x28/-0x1c。
    //   ORIG 0x0826ef52 `movsbl -0x19(%ebp),%eax; mov %eax,-0x2c(%ebp)` 把
    //   serverEnv 放 CMyFileLog **之下**，4.4 恒放其上方 ⇒ 4 槽向 + 帧 16B 属
    //   N126（版本首槽基址常数）+ N115（outgoing 预留）族。三值 register 保活
    //   (esi/ebx/edi) 已命中 P231，仅余寄存器名轮换（P230/N7）。
    flog(pData,
         "[RENT] Dimension m_id(%d) char_no(%d) server_info(%d) item_id(%d)",
         accId, charNo, serverEnv, itemId);
