// DynamicPool<SIPGData>::CNode 特化 ctor（ORIG W 0x081021d6，声明见 TaiwanCash.h
// 全局作用域）：基类 value-init（POD 聚合清零）→ 4.4 -O0 发射与 ORIG 同形的
// `mov $0x0,%eax; mov $0xb7,%edx; rep stos`（探针 /tmp/bm3/p20 N1）；随后两个
// 成员各带 this 重读的函数体赋值（ORIG `mov 0x8(%ebp),%eax; movl $0x8f21,0x2dc`
// `/ movb $0,0x2e0` 形，P29 函数体序）。
DynamicPool<WongWork::IPG::SIPGData>::CNode::CNode()
    : WongWork::IPG::SIPGData()
{
    m_type = 0x8f21;
    m_inUse = 0;
}