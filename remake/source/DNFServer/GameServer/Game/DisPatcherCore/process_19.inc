// ORIG 0x081c5904 T process（process.md 全文转写）。
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 136（ORIG 192 / ours 196），残差=P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
int DisPatcher_MoveItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    // 本 TU 同址视图（PvPTimeout::read ViewPacked 先例）：m_fromSpace/m_toSpace 以
    // unsigned char 读出 ORIG `movzbl 0xd/0x18(%eax),%edx` 单步 disp 折叠 + QI 直存
    // （冻结头 char 声明混号必两段）；偏移/宽度按 PacketTypes.h:908 布局注释。
    #pragma pack(push, 1)
    struct MoveItemSpaceView {
        char m_head[0xd];
        unsigned char m_fromSpace;   // +0x0d
        short m_fromPos;             // +0x0e
        unsigned int m_count;        // +0x10
        int m_field14;               // +0x14
        unsigned char m_toSpace;     // +0x18
        short m_toPos;               // +0x19
        unsigned int m_itemIdx;      // +0x1b
        char m_gap1f[4];             // +0x1f..0x22
        char m_expanda;              // +0x23
        char m_expandb;              // +0x24
    };
    #pragma pack(pop)
    const MoveItemSpaceView* req = reinterpret_cast<const MoveItemSpaceView*>(&msg);
    stMoveItemParam& out = static_cast<stMoveItemParam&>(param);
    // [b28d-solo-DisPatcherCore] P211：ORIG value→%edx 先、槽地址→%eax 后
    // （RHS-first+edx 中转）=「unsigned 返回 + 无具名 err/dst 局部」形。
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        if (out.m_4 == 0xd5)   // 仅锁定类错误回显源/目的 space
        {
            out.m_8 = req->m_fromSpace;
            out.m_10 = req->m_toSpace;
        }
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x141b, __PRETTY_FUNCTION__, out.m_4, user->get_acc_id());
    }

    // ORIG 无 outSlot/ret 初始化写（ret 槽由 call 返回值直写 `mov %eax,-0x1c`）。
    int outSlot;
    // [DPCB 实验 2026-09-11] RHS 求值序旋钮：ORIG 0x081c5b46 为
    // `mov -0x1c(%ebp),%edx; mov -0x20(%ebp),%eax; mov %edx,0x4(%eax)`（RHS 先载
    // %edx、LHS 基址后载 %eax），4.4 对「同型 VAR_DECL → int 成员」发 LHS-first。
    // 试「ret 声明为 unsigned」引入 int←unsigned CONVERT 逼出 RHS-first。
    unsigned int ret;
    // ORIG +0x0ad：uchar 装载 + 字节立即数 `cmp $0x3,%al` 直判（无 sext）。
    if ((req->m_toSpace == 3 || req->m_toSpace == 0x12) &&
        req->m_toPos > 9 && req->m_toPos < 0x16)
    {
        // 扩容区间移动（toSpace 3/0x12 且目的槽位 10..21）
        ret = user->exchange_interspace_expand(
            (int)(char)req->m_fromSpace, (int)req->m_fromPos,
            (unsigned long)req->m_count, (int)req->m_field14,
            (int)(char)req->m_toSpace, (int)req->m_toPos,
            (unsigned long)req->m_itemIdx,
            &outSlot, req->m_expanda, req->m_expandb);
    }
    else
    {
        ret = user->exchange_interspace(
            (int)(char)req->m_fromSpace, (int)req->m_fromPos,
            (unsigned long)req->m_count, (int)req->m_field14,
            (int)(char)req->m_toSpace, (int)req->m_toPos,
            (unsigned long)req->m_itemIdx,
            &outSlot);
    }

    if (ret == 0)
    {
        // 冻结头 m_fromSpace/m_toSpace 为 char；ORIG QI 直存 `movzbl mem,%edx`
        // 要求 RHS 与 m_8/m_10(uchar) 同型（probe /tmp/qi3 s4/s6），经同址
        // uchar 视图读取（偏移不变，§4.3 同型口径）。
        out.m_8 = req->m_fromSpace;
        out.m_a = req->m_fromPos;
        out.m_c = (int)req->m_field14;
        out.m_10 = req->m_toSpace;
        out.m_14 = outSlot;
    }
    else
    {
        // [G5-ProcB] ORIG 0x081c5b46 `mov -0x1c(%ebp),%edx; mov -0x20(%ebp),%eax;
        // mov %edx,0x4(%eax)` —— 单条 `out.m_4 = ret;` 即得「value 先载 edx、
        // 地址槽后载 eax」形。旧文用 `int rv = ret; stMoveItemParam* dst2 = &out;`
        // 造出 2 个具名槽（我方多 5 条 store/load、帧 0x7c vs ORIG 0x6c）。
        // [H5DisPatcher 2026-09-11 核账] 实测原形 `out.m_4 = ret;` 得 **LHS-first**
        // （`mov -0x20,%eax; mov -0x1c,%edx; mov %edx,0x4(%eax)`），与 ORIG RHS-first
        // 相反；加 `(int)` 转换试触发 RHS 先求值**实测无效**（ours 192→192，verdict
        // 仍 NEAR）⇒ 已回滚留原形，本处记 N76/N18a 封顶。
        out.m_4 = ret;
        out.m_8 = req->m_fromSpace;
        out.m_10 = req->m_toSpace;
    }
    return 0;
}
