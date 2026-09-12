// ----------------------------------------------------------------------------
// EventAlttle::ArrangeAlttleEventItem —— ORIG T 0x083fb595 (size 0x2ed)
// [b28d-solo-WongWorkEvents_A] AE（推翻 b21「N 封顶」判词，188=188）：
// ① P227/P139 错误路径改全表达式临时链 + __PRETTY_FUNCTION__（恢复
//    NumberToString→%ebx 跨 ctor 保活 + push %ebx/sub $0xf4）；
// ② P311 反向 struct 行数组（每列两步 `shl $4; lea -0x8(%ebp); lea; sub $K`）；
// ③ P171/P232 补循环体内 dead 局部 `unsigned int unused = 0;`（ORIG
//    `movl $0,-0x10(%ebp)`，把 db/outCount/row/列局部整体下移 4 槽）；
// ④ P225 尾测 while + `if (!fetch()) break;`。
// 取 occ_check!=0 行（limit 10），对 item_check==0 且 server_id 匹配的行收集，
// 之后逐条 Update + InsertEventItem2EventTable(event_code=6)。
// ----------------------------------------------------------------------------
void EventAlttle::ArrangeAlttleEventItem(memberIdentificationNumber_t mid, unsigned int serverId)
