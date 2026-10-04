// ---------------------------------------------------------------------------
// _ProcessIPGError —— ORIG T 0x083237f6 (0x182)
// 【§6.2】P167 差集归零；AtoiAt/ReadIntAt/BufAt 已宏化（P49），109 vs 105
// 残余为寻址折叠/寄存器形态（N7/N18b 族）。
// resData->status(+0x1c)==2 且 atoi(resData+0x173)==1 时走 helper 取消链；
// 否则 logFormat "IPG ERROR (Goods No: %d)(%s)"；末尾 user!=0 转 ProcessError。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_CeraShopDeps] verdict=DIFF-N 封顶（orig 109 / our 110，gap=1；
// p167 CLEAR 3/3）。本轮修法：P59/P222 分支极性翻转（ORIG 0x08323837 `je <user==0
// 块>`，then 为 9 参重载 slot7、else 为 2 参重载 slot9 + return 直跳 epilogue）、
// P235 具名成员访问（resData->m_type 消 ReadIntAt 宏的 add+mov）、P319 整型加法
// BufAt 复现 add+mov 并使虚指针回落栈槽（与 ORIG `call *-0x28(%ebp)` 同形）、
// P223 负条件 goto 尾空标签复现 4.1 if-join 岛（`call ProcessError; jmp; nop`）。
// 残余唯一 1 条：logFormat 0x177 实参 4.4 恒插 `mov %eax,%edx` 中转
// （int 形参/const char* cast 两形实测不消）⇒ 语义等价，byte 层 N 封顶。
// [M2FreeNext15A · 2026-09-11] 符号轴修复：logFormat 第 3 实参原为手写签名字符串，
// 已改回 __PRETTY_FUNCTION__（P139，§6.5 盲区）。直证：ORIG 0x08c26c00 存在具名局部
// 静态符号 `_ZZN8WongWork9CCeraShop16_ProcessIPGErrorEP5CUserPNS_3IPG8SIPGDataES5_hijiicE19__PRETTY_FUNCTION__`
// （size 0x99=153）；复编后我方 .o 发射同名同 size 符号、.rodata 逐字节相同
// ⇒ 消除该「仅ORIG」签名串符号（AE 归一化看不见，四数不变：TU 61/119/0/4 前后逐值相等）。
// [P8S3 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
void CCeraShop::_ProcessIPGError(CUser* user, IPG::SIPGData* reqData,
                                 IPG::SIPGData* resData, unsigned char errCodeA,
                                 int errCodeB, unsigned int productNo, int subCode,
                                 int code2, char flag)
{
    // [b28d-S278] P235 具名成员访问消 add：ORIG 0x0832380e `mov 0x1c(%eax),%eax`
    // 单条 disp（ReadIntAt 宏的指针算术 4.4 恒发 add+mov）。
    if (resData->m_type == 2)
    {
        if (atoi(resData->m_cmd.m_output.m_resultCode) == 1)
        {
            // [b28c-P59/P222] ORIG 0x08323837 `je <user==0 块>`：then 为 user!=0
            // 的 9 参重载（vptr+0x1c slot7），else 为 user==0 的 2 参重载
            // （vptr+0x24 slot9，TaiwanCash.h:239/245 槽序实证）+ return 直跳
            // epilogue（+0x17a），故写正向 if(user!=0)/else{...;return;}。
            if (user != 0)
            {
                GlobalData::s_pIPGHelper->IPGOutputCancel(
                    user,
                    resData->m_cmd.m_output.m_pad159,
                    reqData->m_cmd.m_output.m_price,
                    reqData->m_cmd.m_output.m_pay,
                    reqData->m_cmd.m_output.m_arg7D,
                    reqData->m_cmd.m_output.m_desc91,
                    reqData->m_cmd.m_output.m_descF5,
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
            // [N1A 2026-09-11 · DIFF(109/110,gap−1)→AE(109/109,gap 0)]
            // 旋钮：第 6 vararg 实参去掉 `static_cast<int>(productNo)`（同型
            // cast 生成的 NOP_EXPR 使 4.4 改走「先求值全部 vararg、后统一
            // store」调度）⇒ 复现 ORIG 0x083238f2 `mov %eax,0x18(%esp)` 直存
            // （旧形多 `mov %eax,%edx` 中转 + `mov %edx,0x18(%esp)`，即
            // 0x083238f2 处 our 109+1）。%d 传 unsigned int 值域不变，语义等价。
            LogManager::logFormat(
                1, "CeraShop.cpp", __PRETTY_FUNCTION__,
                0xb47, "IPG ERROR (Goods No: %d)(%s)",
                productNo, resData->m_cmd.m_output.m_message);
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
}