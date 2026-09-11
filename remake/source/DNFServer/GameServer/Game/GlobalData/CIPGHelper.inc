// W7_BuildHarmonize 去重：CIPGHelper::{Initialize,Finalize} 唯一 owner 为
// W6_IPGDeps.cpp（ORIG 0x08100b3e/0x08100c92 完整转写，Finalize 先转发
// m_mgr->Finalize()）。此处原重复定义已移除。
WongWork::IPG::CIPGHelper::CIPGHelper()
{
    // ORIG 0x082a7190：IIPGHelperInterface 基类（vptr 自动）。
    m_mgr = 0;
}