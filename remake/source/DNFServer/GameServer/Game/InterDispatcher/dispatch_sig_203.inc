    // [b28d-solo-InterDispatcher] P225 while+break 复现 4.1 bool 物化（ORIG 0x084dd1ee
    //   `test %eax,%eax; setne %al; test %al,%al; je`；具名 bool 局部 4.4 恒落槽 +3）。
int Inter_LoadNoCacheData::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user != 0)                                   // ORIG +0x06 cmpl $0x0,0xc; je tail
    {
        // [b28d-solo-InterDispatcher] P226 语句表达式：复现 4.1 的 bool 物化 + 直测
        // （`test %eax,%eax; setne %al; test %al,%al; je`，无栈槽）；具名 bool 局部恒落槽。
        if (({ user->getCurCharacR() != 0; }))       // ORIG +0x17 test %eax; setne %al
        {
            SIG_LOAD_NO_CACHE_DATA* load =    // ORIG +0x23 物化 -0xc
                reinterpret_cast<SIG_LOAD_NO_CACHE_DATA*>(data);
            if (({ user->get_charac_no(-1) == load->m_characNo; }))  // ORIG +0x39 cmp %edx,%eax; sete %al
            {   // ORIG +0x5a~：结果免槽直链，vptr+0x4 = CExpandEquipslot::loadData(user, recv+4)
                user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)1)->loadData(
                    user, load->m_pad4);
            }
        }
    }
    return 0;
}