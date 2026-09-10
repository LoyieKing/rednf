int Inter_SetFactoryHubUser::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // ORIG 084dcff4..084dd070：单出口（尾 `mov $0x0,%eax` 共享）⇒ 源形为
    // `if (user != 0) { ... } return 0;`（非 early-return：后者产 jne+内联 return 两段）。
    if (user != 0)
    {
        // ORIG 084dcffe/084dd001：pPkt 槽在 user 非空门之后物化（-0xc）。
        Inter_SetFactoryHubUserPktView* pPkt = reinterpret_cast<Inter_SetFactoryHubUserPktView*>(data);
        // ORIG 084dd00d `je +0x86`：je 目标=DB 支（084dd072 strncpy/makeRequest），
        // 落空侧=hack 支（084dd00f SetETC/addServerHackCnt）⇒ 源形 then=hack、else=DB。
        if (pPkt->m_2d != 0)
        {
            // ORIG 084dd032 `mov %eax,%esi`（SetETC 结果跨 getHackAnalyzer 调用保活，
            // 无栈槽 ⇒ 具名局部须 register，否则占 -0xc 并把 pPkt 挤到 -0x10）。
            // ORIG 0x084dd032 `mov %eax,%esi` 单段直落（SetETC 返回 int → unsigned 形参无转换）；
            // 声明 int 时 4.4 恒多 `mov %eax,%ebx; mov %ebx,%esi`（P41 变体：实参冗余 cast）。
            register unsigned int etc = user->SetETC((short)pPkt->m_2e,
                                                      (int)pPkt->m_30);
            user->getHackAnalyzer()->addServerHackCnt(
                user,
                (WongWork::ENUM_HACKTYPE)(unsigned int)pPkt->m_2e,
                1, etc, 0);
        }
        else
        {
            char name[29];
            strncpy(name, pPkt->m_f, 0x1d);
            DB_Find_Factory_Hub_User::makeRequest(
                user->GetUID(), name,
                pPkt->m_2e);
        }
    }
    return 0;
