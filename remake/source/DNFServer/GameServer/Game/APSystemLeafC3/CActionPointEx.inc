// ----------------------------------------------------------------------------
// CActionPointEx::CActionPointEx —— ORIG W 0x0889566e（无独立报告，objdump
// 转写；调用者 importActionPointSystemEtcScript.md 栈对象 -0x154 同构实证）
//   call _ZN8APSystem12_ActionPointC1Ev      // 基类子对象 [+0x00,+0x0b)
//   movw $0x0,0xc(%eax)                      // +0x0c word = 0
//   movb $0x0,0xe/0xf/0x10(%eax)             // +0x0e/+0x0f/+0x10 = 0
//   call _ZNSt6vectorIiSaIiEEC1Ev @+0x14     // m_itemList 默认构造
//   movl $0x7,0x20(%eax)                     // m_weekday = 7
//   call _ZNSsC1Ev @+0x24 / @+0x28           // m_str24/m_str28 默认构造
// 平铺权威头无法表达基类关系（改布局会破坏 ScriptLeafA2 等按偏移直访的既有
// 调用方），基类清零以等价成员区 memset 复刻；vector/string 由编译器默认构造
// 自动完成（与 ORIG 同为成员构造调用）。已知差异：ORIG 中 +0x0c..+0x10 清零
// 位于 mem-initializer 阶段（先于成员构造），此处位于构造函数体（后于成员构
// 造）；最终对象状态一致。
// ----------------------------------------------------------------------------
CActionPointEx::CActionPointEx()
    : m_ap()          // ORIG 0x0889567c `call _ZN8APSystem12_ActionPointC1Ev`
    , m_mapKey(0)     // ORIG 0x08895684 `movw $0x0,0xc(%eax)`（mem-init：序在 vector C1 之前）
    , m_field0e(0)    // ORIG 0x0889568d `movb $0x0,0xe(%eax)`
    , m_levelMin(0)   // ORIG 0x08895694 `movb $0x0,0xf(%eax)`
    , m_levelMax(0)   // ORIG 0x0889569b `movb $0x0,0x10(%eax)`
    , m_weekday(7)    // ORIG 0x088956b0 `movl $0x7,0x20(%eax)`（S5AradDisp 已落地）
{
    // [b28d-34] 复核维持 DIFF：ORIG 0x0889566e 构造序（基类 call → +0xc/+0xe/
    // +0xf/+0x10 字段 → +0x14 容器 → +0x20 → +0x24/+0x28 string）与
    // APSystemTypes.h 平铺布局妥协互斥（P242 族）；语义等价。
    // [S5AradDisp 2026-09-11 落地两旋钮：ours 57→56（gap −2→−1）]
    //  ① `: m_weekday(7)` mem-init：ORIG 0x088956b0 的 `movl $0x7,0x20(%eax)`
    //     位于 +0x14 容器构造之后、+0x24 string 之前（0x088956ad/0x088956c5）；
    //     具名成员 mem-init 令 4.4 按声明序在成员构造序列内并发（此前在体内=序错）。
    //  ② `m_record.m_mapKey = 0`：ORIG 0x08895684 `movw $0x0,0xc(%eax)` 两条；
    //     原 `&m_bytes02[0x0a]` 取址式恒发 `mov this; add $0xc; movw (%eax)` 三条
    //     （P183 反向）。
    //  残差（判 DIFF 的根因，无源码解）：ORIG 首位 0x0889567c `call
    //  _ZN8APSystem12_ActionPointC1Ev` = 3 条（mov this; mov (esp); call），
    //  我方平铺布局只能 `memset(this,0,0xb)` = 5 条且在成员构造之后发射；
    //  欲同形需 .h 令 CActionPointEx 真正继承 _ActionPoint（越界，未做）。
    // [P6AuditOldN-S1 2026-09-11 负结论 · p167fix call 差集线索] 试形
    //   `new (reinterpret_cast<_ActionPoint*>(&m_field00)) _ActionPoint();`
    //   ⇒ our 56→**60**（gap −1→−5，反劣）：4.4 -O0 不内联 `operator new(size_t,void*)`
    //   （多 `movl $0xb,(%esp); call <operator new>` + `test %edx; je` null 检查），
    //   且 ctor 发射点仍在成员构造之后 ⇒ 已回滚。欲得 ORIG 3 条形需 .h 真继承
    //   （`class CActionPointEx : public _ActionPoint`）—— 属 P7 头部轴，本 TU 越界。
    // [PoolI 2026-09-11 布局正解 · FragC 同步] _ActionPoint 已是真子对象成员
    // （m_ap，packed 0xb @+0x00），C1Ev 由编译器随 mem-init 列表自动发射；
    // +0xc/+0xe/+0xf/+0x10 四写法改走 mem-init（ORIG 把 immediate store 发在
    // vector C1 之前；函数体赋值会落到成员构造之后 ⇒ 顺序反）。
    // 旧 memset(&m_field00,0,0xb) / m_record.m_mapKey=0 / m_bytes02[0x0c/0x0d/0x0e]=0
    // 随头改（成员 m_field00/m_record/m_bytes02 已移除）全部删除。
}