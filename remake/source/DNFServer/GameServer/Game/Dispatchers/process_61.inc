// ORIG 0x081d6786（报告 Dispatcher_Clear_Used_QP/process.md）：
// ParamBase 实际为 STQuestShopParam，+4 = m_4。
int Dispatcher_Clear_Used_QP::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int err = check_error(user, msg);
    if (err > 0)
    {
        return -1;                       // 0x081d67ae
    }
    if (err < 0)
    {
        return LineFunc(0x4031,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    STQuestShopParam& req = static_cast<STQuestShopParam&>(param);   // -0x18（ORIG 缓存 param 引用）
    MSG_BASE& msgRef = msg;                                           // -0x14（dead 缓存，ORIG 槽位）
    (void)msgRef;
    CQuestShop* shop =
        (CQuestShop*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    if (shop == 0)
    {
        req.m_4 = 1;
        return 0;
    }
    unsigned int ret = (unsigned int)shop->clearUsedQp(user);
    if (ret != 0)
    {
        // N76 封顶：ORIG store 展开序 RHS(ret 槽) 先载、LHS(this) 后载；4.4 恒
        // LHS 先（int=int 同型无转换可借），probe 全试不可翻（范式库 N76）。
        req.m_4 = ret;
        return 0;                                     // ORIG +0xbb mov$0; jmp（分叉 materialize）
    }
    return 0;
}