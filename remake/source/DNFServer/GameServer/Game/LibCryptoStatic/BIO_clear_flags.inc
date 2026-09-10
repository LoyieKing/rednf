int  BIO_clear_flags(bio_st_st* b, int flags);
unsigned long BUF_strlcpy(char* dst, const char* src, unsigned long size);

// ---------------------------------------------------------------------------
// methods_filep —— ORIG .data 0x093f3800 dump 原样镜像：
//   93f3800  02040000 0823de08 70c87e08 a0c77e08
//   93f3810  70ca7e08 50c77e08 e0ca7e08 d0c67e08
//   93f3820  00c77e08 00000000
// 地址换算（t 符号 nm -S 实测）：
//   name="FILE pointer"@0x08de2308, bwrite=file_write@0x087ec870,
//   bread=file_read@0x087ec7a0, bputs=file_puts@0x087eca70,
//   bgets=file_gets@0x087ec750, ctrl=file_ctrl@0x087ecae0,
//   create=file_new@0x087ec6d0, destroy=file_free@0x087ec700,
//   callback_ctrl=NULL
