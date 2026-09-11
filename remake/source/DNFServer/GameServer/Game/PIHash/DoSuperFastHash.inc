// [b28d-L1] 修法：同 char* 版（ORIG W 0x0808c174），结构重写后残余为寄存器/槽位形态差（N7/N82）。P167 CLEAR（0/0）语义等价。
int DoSuperFastHash(unsigned char* data, unsigned int len, unsigned int& hash);
