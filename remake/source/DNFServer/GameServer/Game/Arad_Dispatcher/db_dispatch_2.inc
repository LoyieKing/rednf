// [b29-R28 复核 · N104/N121 封顶] ORIG 0x0818eb0c（158 条）：p167 = 纯 **P93 同址
// 别名伪差**（MISSING `CStreamGuardD2Ev`×2 / `CStreamGuardC2EP6Streamb`×1 vs EXTRA
// `D1Ev`×2 / `C1EP6Streamb`×1，同址多符号 objdump 选名差异）⇒ 调用点多集 CLEAN。
// 指令差仅两处**块序**：①ORIG 正常路径尾 `guard.~D1()` 共享块在函数末（`jmp +0x254`），
// 4.4 把该析构就地内联在成功路径（尾部只留 EH unwind 块）②ORIG `mov $0x0,%ebx`
// 就地发射两次（0x0818ecf5/0x0818ed3e），4.4 cross-jump 合并为一份共享块。
// 二者 = 4.4 block-merge/尾块相对序（N104/N121 族，逐条内容全等）⇒ N 封顶，语义等价。
// 裸 register ret 保留（P248 三条件：跨 call 保活+4.4 不自升+ORIG 用 %ebx）。
int Arad_Dispatcher_BuyItemCheck_Impl::db_dispatch(int code, int uid,
                                                   Stream* stream)
{
    // [b21] 修正真 bug：MsgQueueMgr::put 旧用 DB_Q(=2)，ORIG 传 $0x1 改 DISPATCH_Q
    //   （同族 DB 派发器归队队列 idx 1，与 DBInsertGiveBoxEventReward dispatch 同案）。
    //   残余 N82：BuyItemInfo 五字段 Stream::operator>> 寻址 4.4 用 add/寄存器形 vs
    //   ORIG lea(+off)；日志/__PRETTY 字符串地址为 .o 重定位产物（链接期解析）。语义
    //   闭合封 N。
    // ORIG 0x0818eb0c：读 BuyItemInfo ← stream；GetOutBuffer<MSG_BUY_CERASHOP_ITEM>；
    // arad_sp_buy_item_check 通过后组装 PURCHASE_COUNT_CHECK 内部包入 DB 队列。
    // b28c-L26 清绑（P0/N96 硬约束）：原 `register int ret asm("ebx")` 的
    // **asm 绑定已删**（P0 全域禁绑）。保留裸 register 关键字的依据＝本件双形
    // fresh 实测（orig_insns 158）：
    //   · `register int ret;`  → our 157，行级 diff 85
    //   · `int ret;`           → our 157，行级 diff 135（反劣 +50 行）
    // 本点 ret 为出口码、跨 logFormat/Acquire/put 多次 call 存活，ORIG 各出口
    // 恒 `mov $imm,%ebx` ⇒ 属「值确需跨 call 保活」类，裸 register 令 4.4 以
    // %ebx 免槽承载、装载序回到 ORIG 形；与 lane20A/lane12 的「纯本地条件判定
    // 点裸 register 反劣（帧破 push/pop）」判据不冲突（那两点无跨 call 寿命）。
    register int ret;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)7,
                                                  (ENUM_SERVER_GROUP)0);
    ARAD_DATAS::BuyItemInfo info;
    // b28：ORIG 0x0818eb31 GetDBHandle 后直接 `lea -0x28+0xc >>`——info 无预清
    // call（P131）；原 memset(0x14) 在 4.4 展开 call memset = +5 条伪差，删。
    *stream >> info.mid;
    *stream >> info.characNo;
    *stream >> info.ipgNo;
    *stream >> info.checkType;
    *stream >> info.limitCount;
    MSG_BUY_CERASHOP_ITEM* msg = stream->GetOutBuffer<MSG_BUY_CERASHOP_ITEM>();
    if (msg == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Dispatcher_BuyItemCheck_Impl.cpp",
            __PRETTY_FUNCTION__, 0x66,
            "[ERROR] MSG_BUY_CERASHOP_ITEM GetOutBuffer => MID[%d], CHARIDX[%d], "
            "ITEMIDX[%d], CHECKTYPE[%d]",
            info.mid, info.characNo, info.ipgNo, (int)info.checkType);
        ret = -1;
        goto out;
    }
    // b28：去块包裹+早 goto——4.4 对「块内对象+goto 提前退」恒每臂展开 guard D1
    // （ORIG 0x0818ed3e 直 jmp 尾共享 D1@0x818ed60）；统一尾使 D1 单点发射。
    // chk 用 char* 视图形：ORIG 判空/写均 `mov -0x1c` 直读槽（P183 反向：`(char*)chk`
    // cast 形 4.4 恒重算 lea+1，+1 条）。
    if (DATABASE::ITEM_QUERY::arad_sp_buy_item_check(info) != 0)
    {
        ret = -1;
    }
    else
    {
        // P134：Acquire 内联进 CStreamGuard ctor 消中间槽（ORIG 直 `mov %eax,0x4`）。
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire(
                "localjapan/Arad_Dispatcher_BuyItemCheck_Impl.cpp", 0x71),
            true);
        guard.operator*()->operator<<(code);
        guard.operator*()->operator<<(uid);
        char* chk = (char*)
            guard.operator->()->GetInBuffer<ARAD::INTERNAL_PACKET::PURCHASE_COUNT_CHECK>();
        if (chk == 0)
        {
            LogManager::logFormat(
                1, "localjapan/Arad_Dispatcher_BuyItemCheck_Impl.cpp",
                __PRETTY_FUNCTION__, 0x7b,
                "ITEM_PURCHASE_COUNT_CHECK MID[%d], CHARIDX[%d], ITEMIDX[%d], "
                "CHECKTYPE[%d]",
                info.mid, info.characNo, info.ipgNo, (int)info.checkType);
            ret = 0;
        }
        else
        {
            *chk = info.buyable;
            memcpy(chk + 1, msg, 0x3f84);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            ret = 0;
        }
    }
out:
    return ret;
}