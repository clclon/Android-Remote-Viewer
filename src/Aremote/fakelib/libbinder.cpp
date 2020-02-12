
#include "libfake.h"

namespace android {

class IPCThreadState;

class ProcessState
{
public:
    static   sp<ProcessState> self();
    void     startThreadPool();
    void     spawnPooledThread(bool);
    uint32_t setThreadPoolMaxThreadCount(size_t);

private:
    ProcessState();
   ~ProcessState();
};


class IPCThreadState
{
public:
    static  IPCThreadState* self();
    void    joinThreadPool(bool isMain = true);

private: 
    IPCThreadState();
    ~IPCThreadState();
};

IPCThreadState::IPCThreadState() {
    LOGPF();
}
IPCThreadState::~IPCThreadState() {
    LOGPF();
}
IPCThreadState * IPCThreadState::self() {
    LOGPF();
    return new IPCThreadState();
}
void IPCThreadState::joinThreadPool(bool b) {
    LOGPF();
}

sp<ProcessState> ProcessState::self() {
    LOGPF();
    sp<ProcessState> p;
    p.m_ptr = NULL;
    return p;
}

void ProcessState::spawnPooledThread(bool b) {
    LOGPF();
}

void ProcessState::startThreadPool() {
    LOGPF();
}

uint32_t ProcessState::setThreadPoolMaxThreadCount(size_t z) {
    LOGPF();
    return 0U;
}

ProcessState::ProcessState() {
    LOGPF();
}

ProcessState::~ProcessState() {
    LOGPF();
}
    
}

