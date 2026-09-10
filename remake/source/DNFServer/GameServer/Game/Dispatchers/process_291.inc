int Dispatcher_ImageCommunicationEquipmentUse::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stImageCommunicationEquipmentUse& out =
        static_cast<stImageCommunicationEquipmentUse&>(param);
    // ORIG 槽序：param 引用(-0xc) 只有 out.m_4 具名（无 err 具名）——
    //   0x081e?: `mov -0xc(%ebp),%edx; mov %eax,0x4(%edx)` 直接写回，
    //   随后 `mov -0xc(%ebp),%eax; mov 0x4(%eax),%eax; test %eax,%eax; je <T>`
    //   读的是 out.m_4 槽。原 `const int err` 多占 -0x14 槽。
    out.m_4 = check_error(user, msg);
    if (out.m_4 == 0)
    {
        // ORIG：价格值经 %ebx 跨调用直传（`mov (%eax),%ebx`），无具名 price 槽
        //   （原 `const int price` 占 -0xc 槽，产生 `mov %eax,-0xc(%ebp)` 往返）。
        user->getCurCharacInvenW()->use_money(
            *(int*)G_CDataManager()->GetImageCommnicationData(),
            (eMoneySubReason)0x30, true);
        out.m_8 = *(int*)G_CDataManager()->GetImageCommnicationData();
        out.m_c = user->getCurCharacMoney();
    }
    return 0;
