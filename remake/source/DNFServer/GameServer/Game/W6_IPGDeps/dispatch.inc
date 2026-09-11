// ORIG T 0x08105276 (0xcb)：主循环 —— 取请求→同步处理→接口结果回调（vptr+0x34 =
// IIPGHelperInterface slot11 IPGResult，CIPGHelper vtable 0x08b420a0 槽序实证）→
// 释放临时结果与请求；s_server_stop 置位退出。
// b28d-Main 终裁（P282）：ORIG 0x081052dc 段 `mov (%eax),%edx; add $0x34,%edx;
// mov (%edx),%ecx` = vptr+0x34 = slot13 = IIPGHelperInterface::IPGResult
// （TaiwanCash.h:244；vptr+0x2c=slot11 才是 IPGQuery）⇒ 调用目标为
// IPGResult(req, res)（压栈序 arg1=req@0x4/arg2=res@0x8），非 IPGQuery。已上报 Main 裁定。
// [b28d-solo-W6_IPGDeps] verdict=NEAR/N7 封顶（447 口径 58=58，残差仅寄存器角色）。
// 修法：① 内联
// `m_mgr->GetIPGHelper()->IPGResult(req,res)`（ORIG helper 指针无槽往返，P0/P52，
// 消 -0xc 具名槽）；② req==0 判定改 `while(req==0){Sleep;goto next_iteration;}`
// 复现 ORIG `cmpl $0,-0x10; sete %al; test %al,%al; je <Proc>` 的 ==0 物化
// （P225 while 形；4.4 对 if 恒折 jne 直跳）。残差=虚调用目标寄存器 ORIG %ecx/
// 实参 %edx vs 4.4.7 %edx/%ecx（N7）。双后端读数同 Finalize：4.4.6 后端下 AE。
void CIPGMgr::CIPGWorker::dispatch(void* /*arg*/)
{
    SIPGData* req = 0;
    SIPGData* res = 0;
    while (!GlobalData::s_server_stop)
    {
        req = m_mgr->_GetIPGRequest();
        // [b28d-solo-W6_IPGDeps] ORIG 0x081052a0 `cmpl $0,-0x10; sete %al;
        // test %al,%al; je <Proc>` = 4.1 的 ==0 bool 物化；4.4 对 if 恒折 jne
        // 直跳。按 P225（while 形复现 setcc 物化）用 while+goto 尾标签形。
        while (req == 0)
        {
            OS_API::Sleep(10);
            goto next_iteration;
        }
        res = ProcIPGSync(req);

        // [b28d-solo-W6_IPGDeps] ORIG 0x081052d7 段：GetIPGHelper() 结果留 %eax
        // 直作 receiver（无具名局部槽往返）⇒ 内联链式调用（P0/P52）。
        // ORIG vptr+0x34 = slot13 IPGResult(req, res)（vtable 0x08b420a0 逐槽实证）。
        m_mgr->GetIPGHelper()->IPGResult(req, res);
        if (res != 0)
        {
            m_mgr->FreeIPGData(res);
            res = 0;
        }
        m_mgr->FreeIPGData(req);
next_iteration:;
    }
}
