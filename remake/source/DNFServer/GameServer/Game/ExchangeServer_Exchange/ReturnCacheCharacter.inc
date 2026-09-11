// ---------------------------------------------------------------------------
// ReturnCacheCharacter                     ORIG 0x84918d8
// 本 channel 命中缓存并回包；参数 e 控制命中后是否归还池内存。
// ---------------------------------------------------------------------------
// [b28e 2026-09-09] 126/127（p167 CLEAR）：P231 纯 register bool 复现 ORIG 的
// `&&` 短路物化岛（P226 语句表达式对内存第二条件被 4.4 jump-thread，探针 p1-t2；
// register 形探针 p7-t47 出岛，仅多 1 条 `mov %eax,%ebx`，ORIG 值在 %al）。
// 真 bug 已修：GetCacheCharacter 实参 c→d。
// 残差=块内局部槽向（pCache -0x34 vs -0x14、trace -0x30/-0x20 vs -0x2c/-0x3c，
// N93 附）+ 1 条寄存器中转 mov 封顶。
// [K2 2026-09-11 复核] 残差收窄为唯一 1 条（126/127）：`register bool valid` 的物化
// 多 1 条 `mov %eax,%ebx`（ORIG 岛尾 `mov $0/$1,%eax` 后直接 `test %al,%al`）。
// 探针 /tmp/k2p/boolmat.cpp v1..v7/w1..w5 八形实测：`if (a && b)` 四形全被 4.4
// jump-thread（无岛）；要岛必须给具名变量 ⇒ 落槽形 +2、register 形 +1（无 0 成本形）
// ⇒ N18a/N7 封顶。槽向残差已随 b28e 修消失（现两侧 -0x34/-0x30/-0x20 全等）。
bool CExchangeServer::ReturnCacheCharacter(int a, int b, int c, int d,
                                           bool e, CSession* session)
{
    // [b28d-solo-Exchange] ORIG 0x8491926 `test %al; je <else>` 正极性 ⇒
    // `if (GetCacheCharacter(...)) { 成功 } else { trace 0x1d6 }`（旧码取反写、
    // 且具名 mgr 多槽，P0 应内联）。P226 语句表达式复现 valid 的零槽短路物化
    // （ORIG 0x8491963-0x849197f，valid 无槽往返）。
    bool handled = false;
    if (GetServerChannelNo() == c)
    {
        CACHE_CHARACTER_TYPE* pCache = NULL;

        // [b28d-solo-Exchange 真 bug] ORIG 0x8491917 压 `0x18(%ebp)` = 第 4 形参
        // d（charac_no），旧码误传 c（channel_no）；比较 `== c` 才是 channel_no。
        if (GetInstanceCacheCharacterMgr()->GetCacheCharacter(d, &pCache, e))
        {
            int pos = 0;
            int memberIdx = 0;
            bool found = false;
            pos = pCache->GetMemberPosition((ENUM_INTERNALPACKET)a,
                                            &memberIdx, found);
            // [b28e] P226 语句表达式对「内存字段」第二条件被 4.4 jump-thread（探针
            // /tmp/probe_es p1-t2）；ORIG 0x8491963..0x849197f 为 && 结果物化岛
            // （mov$1/jmp/mov$0/test）且无槽 ⇒ P231 纯 register 存储类提示复现
            // （探针 p7-t47：岛形一致，仅 test 寄存器 %bl vs ORIG %al）。
            // [b28e 本泳道复测] 去 register（plain bool）⇒ 4.4 落槽物化
            // `mov %al,-0x9; cmpb $0,-0x9`（ORIG 为 `test %al,%al` 零槽）
            // ⇒ 保留 register 形（不占槽、指令数同）。
            register bool valid = (pos != 0 && found);
            if (valid)
            {
                if (session != NULL)
                {
                    SendResponseCharacInfo(a, b, (void*)pos, memberIdx,
                                           session, 0);
                }
                else
                {
                    SendResponseCharacInfo(a, b, (void*)pos, memberIdx);
                }
                handled = true;
            }
            else
            {
                // [b28e 本泳道] P221/P227 全表达式临时链：ORIG 0x84919ec/0x8491a3e
                // 两处 trace 分别占 -0x30/-0x20（浅于 pCache -0x34）；具名对象形在
                // 4.4 取反方向（-0x2c/-0x3c）⇒ 改构造即调用临时链。
                cMyTrace(__PRETTY_FUNCTION__, 0x1bf, 0)(
                    "GetMemberPosition() invalid information  type(%d), "
                    "idx(%d), channel_no(%d), charac_no(%d)", a, b, c, d);
            }
            if (e)
            {
                GetInstanceCacheCharacterMgr()->FreeCharacter(pCache);
            }
        }
        else
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1d6, 0)(
                "GetCacheCharacter() false  type(%d), idx(%d), "
                "channel_no(%d), charac_no(%d)", a, b, c, d);
        }
    }
    return handled;
}