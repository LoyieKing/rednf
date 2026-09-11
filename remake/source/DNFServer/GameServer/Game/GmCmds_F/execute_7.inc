// ORIG execute 0x084a9252：GetUser()->RewardTutorial(flag)；flag 为 unsigned
// 局部（RewardTutorial 形参 unsigned int，ORIG mangled 尾 j）。
// 帧差 0x10 用 P190 double 对齐撑帧闭合（探针 /tmp/bl5/probe3.cpp padD1：
// 单 double framePad 使 sub $0x14→$0x24 精确命中 ORIG）。
void CSetTutorialFlagCmd::execute()
{
    double framePad;                                    // P190：对齐撑帧，无读写
    (void)&framePad;
    register unsigned int flag = m_flag;
    GetUser()->RewardTutorial(flag);
}