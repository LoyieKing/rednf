// ---- PEM_ASN1_read_bio（ORIG T 0x087f0ba0）。
//      PEM_bytes_read_bio 链就地展开：BIO_gets 剥头尾行、base64 解码、回调
//      d2i(ptr,&buf,len) 完成 ASN1 侧解析；头名行需包含 name 匹配。cb/u
//      （解密回调）取 cb!=0 时 ORIG 抛错分支忠实保留(ERR 9/13/103)。----
long BIO_gets(void* bp, char* buf, int size);        // ORIG 公开符号（下定义）
