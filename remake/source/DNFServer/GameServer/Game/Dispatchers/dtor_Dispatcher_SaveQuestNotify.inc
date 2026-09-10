Dispatcher_SaveQuestNotify::~Dispatcher_SaveQuestNotify() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [S206 收敛 9→1 块] ORIG 0x0825eec4
//   [N82→P291/P311] 已修：+0xd 区改具名数组成员视图（scaled 索引 `mov 0xd(%eax,%edx,4)`）
//   + find_quest 结果具名局部落槽比较；残余仅 N18a 单段 regmove（ORIG
//   `mov 0xd(%eax,%edx,4),%eax; mov %eax,%ebx` vs 4.4 折 `…,%ebx`，P254 enum 化未过）。
