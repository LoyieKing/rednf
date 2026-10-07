// ORIG 0x081d64b4 T _ZN37Dispatcher_Buy_Charac_Status_Using_QP7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/Dispatcher_Buy_Charac_Status_Using_QP/process.md 汇编+反编译 C 全文转写）
// param 为模板实参 STQuestShopBuyParam（+0x04 byte/+0x06 short/+0x08 int）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d64b4 orig/ours=73/74 ndiff-blocks=13
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x1c(%ebp) | cmpl   $0x0,-0x1c(%ebp) | cmpl   $0x0,-0x1c(%ebp)
int Dispatcher_Buy_Charac_Status_Using_QP::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_QUEST_SHOP& mref = reinterpret_cast<MSG_QUEST_SHOP&>(msg);  // 裸偏移→具名字段（同偏移同宽度）
    int err = check_error(user, msg);
    if (err >= 1)
    {
        return -1;
    }
    if (err < 0)
    {
        return LineFunc(0x3fc9,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG GetCharacExpandData(param_1, 0xb)（扩展数据 0xb 槽）
    CQuestShop* shop = reinterpret_cast<CQuestShop*>(
        user->GetCharacExpandData(static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xb)));
    if (shop == 0)
    {
        static_cast<STQuestShopBuyParam&>(param).m_8 = 1;
        return 0;
    }
    short result = 0;   // local_22
    int ok = shop->buyStatus(user,   // ORIG 返回码 0/1/0xe（param+8 需完整值）
                             mref.m_0d,
                             mref.m_0e,
                             result);
    // [DNF-FIX-QPSKILL 2026-10-07] ORIG 0x081d658c `cmpl $0,-0xc; je 0x081d65a2`：
    //   ret!=0（失败）→ 0x081d6598 param+0x8 = ret（send 走失败包 byte0+short err）；
    //   ret==0（成功）→ 0x081d65ac param+0x4 = msg+0xd、0x081d65b6 param+0x6 = result。
    //   旧形两分支体对调：失败被当成功回包（01 status 0），真成功反而回 status/level=0。
    if (ok)
    {
        static_cast<STQuestShopBuyParam&>(param).m_8 = ok;
        return 0;
    }
    STQuestShopBuyParam& out = static_cast<STQuestShopBuyParam&>(param);
    out.m_4 = mref.m_0d;
    out.m_6 = result;
    return 0;
}