#ifndef __THREADED_CALLBACK_INVOKATION_H__
#define __THREADED_CALLBACK_INVOKATION_H__
#include <uv.h>

#ifdef __cplusplus

class callback_info;
/**
 *   Synchronization object to ensure following order of execution:
 *   -> WaitForExecution()     invoked
 *   -> SignalDoneExecuting()  returned
 *   -> WaitForExecution()     returned
 *
 *   ^WaitForExecution() must always be called from the thread which owns the object
 */

class ThreadedCallbackInvokation {
  public:
    ThreadedCallbackInvokation(callback_info *cbinfo, void *retval, void **parameters);
    ~ThreadedCallbackInvokation();

    void SignalDoneExecuting();
    void WaitForExecution();

    void *m_retval;
    void **m_parameters;
    callback_info *m_cbinfo;

  private:
    uv_cond_t m_cond;
    uv_mutex_t m_mutex;
};


#endif

#endif
