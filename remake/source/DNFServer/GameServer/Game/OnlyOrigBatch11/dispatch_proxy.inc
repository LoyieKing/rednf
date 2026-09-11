// ORIG T 0x08630c1a：`mov 0x8(%ebp),%eax; mov %eax,-0xc`（self 具名局部有槽
// 往返，P0）→ `mov -0xc,%eax; mov (%eax),%eax; add $0xc,%eax; mov (%eax),%edx`
// （虚表槽 3）→ (esp)=self（从槽）、0x4=形参槽 → `call *%edx` → `mov $0,%eax`。
void* ThreadInterface::dispatch_proxy(void* p)
{
    ThreadInterface* self = static_cast<ThreadInterface*>(p);
    self->dispatch(p);
    return 0;
}