// ORIG T _Z21_NS_NonClient_RequestP5CUser @0x08274bc5（0xfe，报告缺失，objdump 逐指令转写）：
// rand 序号 → put_header(0,0x110)/put_int/rand → Send → SetNonClientRandInt →
// G_TimerQueue()->InsertTimer(0,GetUID,0x8c,60,get_unique_id,0xffffffff)。
// [b28d-lane2] verdict=DIFF ndiff=15（ORIG 0x08274bc5）：混合残差；gap+1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=17（ORIG 0x8274bc5）：P30/N4 add-vs-lea。
// [b28c-L23 N(块序/jump threading) 残余 16 行] 唯一结构差 = PacketGuard 正常出口
// 块的位置：ORIG @0x08274c... 在 `call InsertTimer` 之后发 `jmp <统一尾>`，把
// `~PacketGuard / add $0x30 / pop×3 / ret` 留在**异常清理块之后**（4.1 为 EH
// landing pad 生成共享尾），4.4 因正常路径唯一而就地展开析构、把 EH 块推到函数尾
// ⇒ 同指令同序、块位置互换（68/69 条数差 1 源于 4.1 的额外 jmp）。压参序、
// [复原] 该块序可用「guard 包进内层作用域块 + 块为函数体末语句」复现（/tmp/probe_eh.cpp
//   f4 探针：4.4 -O0 对 `{ G g; … }` 块尾同样发 `jmp L; <unwind>; L: ~G + epilogue`）。
void _NS_NonClient_Request(CUser* user)
{
    const int randInt = rand();
    PacketGuard packet;
    packet.put_header(0, 0x110);
    packet.put_int(randInt);
    packet.finalize(true);
    user->Send(packet);
    user->SetNonClientRandInt(randInt);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, (int)user->GetUID(),
                                (TIMER_MESSAGE)0x8c, 60,
                                (unsigned short)user->get_unique_id(), 0xffffffff);
    // [实测 2026-09-10] ORIG 的 `jmp <共享尾>`（同指令同序、仅块位互换）在本函数未能用
    // 裸 return / 块+return / goto 尾标签复现（探针 f6/f9 在小函数命中、本函数恒就地展开）
    // ⇒ 维持 N104 记法，待后续与同类件统一复算。
}