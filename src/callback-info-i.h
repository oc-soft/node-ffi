#ifndef __CALLBACK_INFO_I_H__
#define __CALLBACK_INFO_I_H__

#include <memory>
#include <ffi.h>

namespace Nan {
class Callback;
}
/*
 * One of these structs gets created for each `ffi.Callback()` invokation in
 * JavaScript-land. It contains all the necessary information when invoking the
 * pointer to proxy back to JS-land properly. It gets created by
 * `ffi_closure_alloc()`, and free'd in the closure_pointer_cb function.
 */

class callback_info : public ffi_closure {
  void *code;                    // the executable function pointer
  std::unique_ptr<Nan::Callback> errorFunction;    // JS callback function for reporting catched exceptions for the process' event loop
  std::unique_ptr<Nan::Callback> function;         // JS callback function the closure represents
  // these two are required for creating proper sized WrapPointer buffer instances
  int argc;                      // the number of arguments this function expects
  size_t resultSize;             // the size of the result pointer
public:
  /**
   * constructor
   */
  callback_info();

  /**
   * destructor
   */
  ~callback_info();

  /**
   * set argc
   */
  void
  SetArgc(
    int argc);

  /**
   * get argc
   */
  int
  GetArgc() const;

  /**
   * set result size
   */
  void
  SetResultSize(
    size_t resultSize);

  /**
   * get result size
   */
  size_t
  GetResultSize() const;

  /**
   * get js callback function for reporting catched exceptions
   */
  const std::unique_ptr<Nan::Callback>&
  GetErrorFunction() const; 

  /**
   * set js callback function for reporting catched exceptions
   */
  void 
  SetErrorFunction(
    std::unique_ptr<Nan::Callback>& errorFunction); 

  /**
   * get js callback function the closure represents
   */
  const std::unique_ptr<Nan::Callback>&
  GetFunction() const;

  /**
   * set js callback function the closure represents
   */
  void 
  SetFunction(
    std::unique_ptr<Nan::Callback>& function); 

 /**
  * get the code to call function
  */
  void*
  GetCode() const;

 /**
  * set the code to call function
  */
  void
  SetCode(
    void* code);

  /**
   * free allocated object by ffi_closure_alloc
   */
  static void
  Free(callback_info* info);
};

#endif
