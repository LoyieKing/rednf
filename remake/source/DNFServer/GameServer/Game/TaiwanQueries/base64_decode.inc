void base64_decode(std::string const& encoded_string,
                   std::vector<unsigned char>& out);

// 说明：ORIG GarenaAuthPCRoom 只取 CUser::GetUID()/get_acc_id() 两个 const
// 访问器。因上游 CUser.h→CDataManager.h→CItemGenerator.h 链正被其它批改造
// （2026-08-27 波动期），此处采用【最小只读类视图】生成对真实 ORIG 符号
// _ZNK5CUser6GetUIDEv/_ZNK5CUser10get_acc_idEv 的常规成员调用（ABI 即
// const thiscall、this-only，指针传递不变）；调用语义与签名以 CUser.h 权威
// 声明为准，Wave5 收尾可直接换回 #include "CUser.h"。
