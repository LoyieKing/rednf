// ncl_make_mac(char*) —— 本机 eth0 MAC，每字节 "%03d"（3 位十进制）拼接。
// ORIG：socket(AF_INET, SOCK_DGRAM, 0) → struct ifreq(栈 0x30) memset →
//   memcpy 设备名 "eth0"（5 字节含 NUL）→ ioctl(SIOCGIFHWADDR=0x8927) →
//   close → 6 字节各 sprintf("%03d") 到 out，每 3 字符后补 '\0'（rodata
//   0x8ca6a10="eth0" / 0x8ca6a15="%03d"）。
void ncl_make_mac(char* out)
