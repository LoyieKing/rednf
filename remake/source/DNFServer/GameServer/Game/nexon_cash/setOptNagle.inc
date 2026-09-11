// 返回 setsockopt()==0。ORIG 反汇编：结果先存栈变量再 cmpl+sete 返回。
bool TCPSocket::setOptNagle(bool on)
