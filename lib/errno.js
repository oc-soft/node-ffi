var DynamicLibrary = require('./dynamic_library')
  , ForeignFunction = require('./foreign_function')
  , bindings = require('./bindings')
  , ref = require('ref')
const errno = bindings.Errno

if (process.platform == 'win32') {
  const ucrtBase = DynamicLibrary('ucrtbase.dll');
  const int = ref.types.int
  const intPtr = ref.refType(int, true)
  const _errno = ucrtBase.get('_errno')
  const errnoPtr = ForeignFunction(_errno, intPtr, [])
  let getErrno = function() {
    const resPtr = errnoPtr()
    const res = resPtr.deref()
    return res
  }
  let setErrno = function(errorValue) {
    const resPtr = errnoPtr()
    
  }
}

module.exports = errno
// vi: se ts=2 sw=2 et:
