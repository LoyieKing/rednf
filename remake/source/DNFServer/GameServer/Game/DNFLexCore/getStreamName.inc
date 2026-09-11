// ---- DNFLex::getStreamName()（ORIG 0x8acd310，tstring RVO）----
// [b28d-19] N：N7/N82 —— 同上（get_next_token 同族）；ORIG 0x08acd310。
// 【b28e-M1S2 本波复核】orig 43 / ours 64（我方多 21）。真因 = **GCC 4.4.7 在
// 「函数含多个 return 对象」时放弃 NRVO**，而 ORIG 编译器不放弃：
//   · ORIG 0x08acd330：`lea -0x18(%esi),%eax; sub $0x14,%esi; mov %eax,0x4(%esp);
//     mov %ebx,(%esp); call _ZNSsC1ERKSs`（%ebx = 0x8(%ebp) = sret）⇒ 具名 result
//     直接构造在返回槽，全程无第二次 copy、无局部 dtor；
//   · 我方 0x2f4 起：ctor 到 -0x20(%ebp) → append → `call _ZNSsC1ERKSs(sret,tmp)`
//     → 内联 COW 释放（`lock xadd` 引用计数块）⇒ 恰合 21 条差。
//   同编译器隔离探针（本 TU 全旗标 -O2 -std=gnu++98）：/tmp/probe2.s 的 `g`
//   （单一 return）走 NRVO、`h`（双 return）恒退化；/tmp/probe3.s 把 NONE 支改成
//   具名 `std::string none("NONE"); return none;` 亦退化 ⇒ 无源级形可造。
//   新钥匙核过：P339/P331（-O0 槽轴；本 TU 为 per-source -O2 路由，不适用）、
//   P226/P231/P248（均不触发 NRVO）⇒ 判 N（编译器 NRVO 策略差）封顶。
std::string DNFLex::getStreamName()
{
    stream_t* finish = stream_stack.end().base();
    if (finish == stream_stack.begin().base())
        return std::string("NONE");
    std::string result((finish - 1)->dir);
    result.append((finish - 1)->filename);
    return result;
}