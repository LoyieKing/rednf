void CCeraShop::_ProcessIPGError(CUser* user, IPG::SIPGData* reqData,
                                 IPG::SIPGData* resData, unsigned char errCodeA,
                                 int errCodeB, unsigned int productNo, int subCode,
                                 int code2, char flag)
{
    // [b28d-S278] P235 具名成员访问消 add：ORIG 0x0832380e `mov 0x1c(%eax),%eax`
    // 单条 disp（ReadIntAt 宏的指针算术 4.4 恒发 add+mov）。
    if (resData->m_type == 2)
    {
        if (atoi(BufAt(resData, 0x173)) == 1)
        {
            // [b28c-P59/P222] ORIG 0x08323837 `je <user==0 块>`：then 为 user!=0
            // 的 9 参重载（vptr+0x1c slot7），else 为 user==0 的 2 参重载
            // （vptr+0x24 slot9，TaiwanCash.h:239/245 槽序实证）+ return 直跳
            // epilogue（+0x17a），故写正向 if(user!=0)/else{...;return;}。
            if (user != 0)
            {
                GlobalData::s_pIPGHelper->IPGOutputCancel(
                    user,
                    BufAt(resData, 0x159),
                    BufAt(reqData, 0x69),
                    BufAt(reqData, 0x71),
                    BufAt(reqData, 0x7d),
                    BufAt(reqData, 0x91),
                    BufAt(reqData, 0xf5),
                    static_cast<void*>(0),
                    static_cast<Taiwan::BillTransaction*>(0));
            }
            else
            {
                GlobalData::s_pIPGHelper->IPGOutputCancel(reqData, resData);
                return;
            }
        }
        else
        {
            LogManager::logFormat(
                1, "CeraShop.cpp",
                "void WongWork::CCeraShop::_ProcessIPGError(CUser*, WongWork::IPG::SIPGData*, "
                "WongWork::IPG::SIPGData*, unsigned char, int, unsigned int, int, int, char)",
                0xb47, "IPG ERROR (Goods No: %d)(%s)",
                static_cast<int>(productNo), BufAt(resData, 0x177));
        }
    }

    // [b28d-S278 P223] ORIG 0x08323929 尾部为 4.1 if-join 岛：
    // `cmpl;je <nop 占位>; body; jmp join; nop; join:`。4.4 对正向 if 恒
    // jump-thread ⇒ 唯一复现形 = 负条件 goto 到共享尾空标签。
    if (user == 0)
    {
        goto ipg_error_done;
    }
    ProcessError(user, static_cast<unsigned short>(errCodeA), errCodeB,
                 productNo, subCode, code2, flag);
ipg_error_done:;
