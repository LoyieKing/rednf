// ---------------------------------------------------------------------------
// 6. CSpecialItemRoutingManager::ProcessFailEtc —— ORIG T 0x0860b332
// （报告 CSpecialItemRoutingManager/ProcessFailEtc.md）：路由开启且首个有效
// 成员状态 ∈ {5, 0xc} 时广播 0x11a 失败包（byte 0/失败码/路由 uniqueId）、
// 取消计时器并结束路由。
// [b28c-L20·N-EH布局] fresh orig=86 our=86 gap=0 ndiff=6：唯一真差为成功路径析构落点——
// ORIG 4.1 在 SetEndRouting 后内联「lea;mov;call ~PacketGuard」再 jmp epilogue；4.4.7 让
// 成功路径 jmp 到 EH 岛之后的共享析构段（~PacketGuard 只发射一次于 epilogue 前）。早返形
// （if(party==0)return;）实测劣化 our=87（多一条 jmp）已回退；包体/谓词形不变。函数体主
// 调用序列逐条同 ORIG：IsRoutingState → GetFirstValidMemberState×2（5/0xc 短路，布尔留
// %al 无槽）→ PacketGuard::C1 → put_header(0,0x11a) → put_byte(0) → put_byte(failCode)
// → put_int(GetRoutingItemUniqueID) → finalize(true) → send_to_party → cancelTimer →
// SetEndRouting → ~PacketGuard（+EH 岛 _Unwind_Resume）。P167 实测 16=16 call 集归零。语义等价。
// ---------------------------------------------------------------------------
// [b28d-solo-UserCoreDepsB2] 87/86：唯一残差 = N104 跨版本 EH 尾块相对序。ORIG 正常
// 路径在 SetEndRouting 后就地 `lea;mov;call ~PacketGuard` 再 `jmp epilogue`，unwind 岛
// 居中；4.4 把正常路径 `jmp` 到 unwind 岛之后的**共享**析构段（`~PacketGuard` 一份，
// 正常/unwind 两路共用），且 jump-thread 掉 ORIG 的 +0x122 nop 岛。两形实测：
// ①P223 goto 尾标签串（`if(party==0) goto L_nop; …; goto L_ret; L_nop:; L_ret:;`）
//   产 nop 但多 1 条 jmp（88，劣化）；②goto 跳出 PacketGuard 作用域强制正常路径析构点
//   无变化（86，析构仍外提）。属 GCC 4.4 cleanup-sharing 编译器行为，源码不可择。
// P167 16=16 归零；包体/谓词逐条同 ORIG（ORIG 0x0860b332）。语义等价。
// [L4b 2026-09-11 复核] 权威口径 DIFF 86/86、ndiff=6：残差即上述 dtor 段位移 3 行
// （ORIG 在 SetEndRouting 后就地 `lea;mov;call ~PacketGuard` 再 jmp epilogue；4.4 把
// 正常路径 jmp 到 unwind 岛之后的共享析构段）。历史已试 P223 goto 尾标签 / goto 跳出
// 作用域，均反劣或无效 ⇒ N104 封顶。
void CSpecialItemRoutingManager::ProcessFailEtc(CParty* party, int failCode)
{
    if (party != 0)
    {
        // ORIG（idx102 报告）：IsRoutingState 为假则跳过状态读取；5 或 0xc 任一
        // 命中即可路由。布尔全程寄存器（test %al,%al; je），故谓词内联不具名局部
        // （P0 无槽局部）。
        if (party->IsRoutingState() &&
            (party->GetFirstValidMemberState() == 5 ||
             party->GetFirstValidMemberState() == 0xc))
        {
            PacketGuard packet;
            packet.put_header(0, 0x11a);
            packet.put_byte(0);      // ORIG put_byte(local_18,0)
            packet.put_byte(failCode);
            packet.put_int((int)party->GetRoutingItemUniqueID());
            packet.finalize(true);
            party->send_to_party(packet);
            cancelTimer(party);
            party->SetEndRouting();
        }
    }
}