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
