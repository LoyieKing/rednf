#ifndef DNF_SEGMENTATION_FAULT_SIG_H_
#define DNF_SEGMENTATION_FAULT_SIG_H_

#include "DNFSignal.h"

class CSegmentationFaultSig : public CSignal
{
public:
    CSegmentationFaultSig();
    virtual ~CSegmentationFaultSig();
    virtual void handle(int sig);
};

#endif // DNF_SEGMENTATION_FAULT_SIG_H_
