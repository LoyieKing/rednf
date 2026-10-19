// Auto-generated header stub from DWARF info
// Original path: inc/VMCore/UPVM.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_INC_VMCORE_UPVM_H_H_
#define SECSVR_INC_VMCORE_UPVM_H_H_

#include "inc/common/Types.h"

// sizeof = 124
struct CUPVM : public CVM { // line 43
public:
CUPVM(const CUPVM &arg0);
virtual BOOL Init(); // line 1201
virtual BOOL Run(); // line 802
CUPVM(); // line 35
virtual ~CUPVM(); // line 44
virtual void Release(); // line 1196
};


extern CVMBase * GetUPVM(); // line 26 (inferred)

#endif // SECSVR_INC_VMCORE_UPVM_H_H_
