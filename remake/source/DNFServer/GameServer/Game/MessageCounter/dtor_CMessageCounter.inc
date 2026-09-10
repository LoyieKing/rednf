CMessageCounter::~CMessageCounter() {}

// ORIG IncreaseMessage @0x0856f9c4：命中则 ++count → 64 位累加耗时字节 →
// 滑动平均 avg = avg*((n'-1)/n') + newBytes*(1/n')；未命中插入首记录。
// [R29-1 按 ORIG 报告逐条重写] 六项：
// ① `it->second` 逐次重读（ORIG 6× _Rb_tree_iterator::ptEv，原我方具名引用 `value`
//    只 1 次 ⇒ p167 MISSING 6）；
// ② 比较用正向 `!=`（ORIG `neq`；我方原 `==` 生 `eq` ⇒ EXTRA）；
// ③ bytes 累加为 64 位 `+=`（ORIG 0x0856fa8a `mov;mov;add;adc;mov;mov`；
//    原我方分 low/high + 进位判断多 1 槽 3 指令）；
// ④ `wOld`/`wNew` 两具名 double，声明于 find 之后（ORIG 预清 `fldz;fstpl` ×2 位于
//    find 之后 end 之前，槽 -0x18/-0x10，wOld 更深 ⇒ 先声明）；
// ⑤ 未命中插入记录 = { count=1, bytes=(unsigned long long)bytes,
//    avg=(double)bytes }（ORIG 0x0856fb3e `movl $0x1,-0x7c`、bytes→-0x78/-0x74、
//    `fildll -0x98; fstpl -0x70`）——**原我方写 `{0,0,0,0.0}` 是语义 bug**
//    （首记录丢 bytes/avg、count 从 0 起）；
// ⑥ 平均更新书写序 `m_avg * wOld + (double)bytes * wNew`（ORIG 首条 `fldl 0x14(%eax)`
//    即 avg ⇒ P243 左操作数先 fld）。
