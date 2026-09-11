// clear 定义移到 ctor 之后（NL7 实证：callee 后置时 4.4 unit-at-a-time 不塌缩
// ctor outgoing，保持 ORIG sub $0x18）。
void RequiredItem::clear()
{
    m_itemIdx = -1;
    m_itemCount = 0;
    m_enableFlag = 0;
}