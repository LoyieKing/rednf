// ORIG T 0x086504d8 sz0x11d：频道类型 !=7 时经 BigStreamPool 构造
// header(0x23)+UID 内部流，GetInBuffer<SIG_SAVE_INVENTORY> 后 _GetSaveInvenBase
// 填充、+0x08 字节置 1，投递 DB_Q。反编译见 CUser/SaveMoneyCoin.md。
bool CUser::SaveMoneyCoin()
{
    // P185 guard 作用域块形（ORIG 0x086504d8：guard@-0x14、put 后 `mov $1,%ebx`
    // 真值先于 dtor、epilogue 统一 movzbl %bl——旧平铺形 dtor 岛位置不同）。
    // ==7 走 sete 物化岛（技巧 22）；uid 跨 operator- 存活 %ebx。
    // ORIG 0x86504d8 **ebx 聚合单出口**：假支 `mov $0,%ebx; jmp END`、真支 put 后
    // `mov $1,%ebx`，END epilogue `movzbl %bl,%eax`；uid 跨 operator* 存活同用 %ebx
    // （时序不重叠：uid 在 put 前消费完）。
    register bool rc;   // b28d-solo-CUser：ORIG rc 驻 %ebx（mov $0/%1,%ebx + 尾 movzbl %bl）
    // b28d-solo-CUser：ORIG `sete %al; test %al,%al; je` 无 ch7 槽（P292 非 bool 物化）
    //   ⇒ 内联条件（probe_bool a3）。
    if (G_GameWorld()->GetChannelType() == 7)
    {
        rc = false;
        goto RCEND;
    }
    {
        CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("user.cpp", 0x18cc),
                           true);
        guard.operator*()->operator<<(0x23);
        register int uid;   // b28d-solo-CUser：ORIG `mov %eax,%ebx` 跨 operator* 保活
        uid = (int)GetUID();
        guard.operator*()->operator<<(uid);
        // ORIG 0x8650574-582：取 buffer 前先经一次 operator->（pt）call——源形
        // `guard->GetInBuffer<...>()`（智能指针循环解引用，pt 返回值作 this）。
        SIG_SAVE_INVENTORY* sig =
            ((CStreamGuard*)(void*)guard.operator->())
                ->GetInBuffer<SIG_SAVE_INVENTORY>();
        _GetSaveInvenBase(sig);
        sig->m_saveFlag = 1;            // ORIG movb $0x1,0x8(%eax)（+0x08 标志）
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        rc = true;
    }
RCEND:
    return rc;
}