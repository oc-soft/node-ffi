var DynamicLibrary = require('./dynamic_library')
  , ForeignFunction = require('./foreign_function')
  , bindings = require('./bindings')
  , errno = bindings.Errno
  , ref = require('ref')
  , int = ref.types.int
  , intPtr = ref.refType(int)

if (process.platform == 'win32') {
  var _errno = DynamicLibrary('msvcrt.dll').get('_errno')
  var errnoPtr = ForeignFunction(_errno, intPtr, [])
  errno = function() {
    return errnoPtr().deref()
  }
}

module.exports = errno
