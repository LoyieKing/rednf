WongWork::CMailBox::CMailBox()
{
    // ORIG 0x085579b2：按布局顺序构造 7 个子对象成员；现均为真实成员，由
    // 编译器按声明序（= ORIG 构造序 pool→map→list→iter→keep→vector→map）生成。
}