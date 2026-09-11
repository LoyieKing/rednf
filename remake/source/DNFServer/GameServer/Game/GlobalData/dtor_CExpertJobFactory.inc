// （b28 试 P216 throw() 回滚：4.4 判 ctor pure 后连 call 一起消除，ORIG 有 call，
//  得不偿失；CExpertJobMgr::C1 残差为 N77/P90 value-init 清零段。）
// [b28d-solo-GlobalData] P299/P318：CExpertJobFactory 用户空 ctor 声明+定义已删
// （ORIG 0x0849f68c 为 W 隐式合成）⇒ CExpertJobMgr::C1 恢复 value-init 前置 rep stos。
expert_job::CExpertJobFactory::~CExpertJobFactory()
{
    // ORIG 0x0849f6d2：两个 StaticPool 成员析构。
}