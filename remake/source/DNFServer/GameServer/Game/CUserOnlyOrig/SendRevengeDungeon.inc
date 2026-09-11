// ---------------------------------------------------------------------------
// CUser::SendRevengeDungeon() T 0x08656abc sz0x156
// 报告：docs/class_func_reports/CUser/SendRevengeDungeon.md
// GetRewardType()==1/2 写 type byte（其它 return）；remain=max(0,close-cur)；
// 10 字节 {type,1,remain:int,reserved:int} 经 put_binary(0x1ec 包)发送。
// ---------------------------------------------------------------------------
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08656abc：N115/N82 封顶——全帧槽平移 4
// （ORIG -0x22/-0x18/-0x14 vs 4.4 -0x26/-0x1c/-0x18）；95 vs 97 条。
void CUser::SendRevengeDungeon()
