var DynamicLibrary = require('./dynamic_library')
  , ForeignFunction = require('./foreign_function')
  , bindings = require('./bindings')
  , ref = require('ref')

if (process.platform == 'win32') {
  const ucrtBase = DynamicLibrary('ucrtbase.dll');
  const int = ref.types.int
  const intPtr = ref.refType(int, true)
  const _errno = ucrtBase.get('_errno')
  const errnoPtr = ForeignFunction(_errno, intPtr, [])
  let errno = function() {
    console.log('intPtr')
    console.log(intPtr)
    const resPtr = errnoPtr()
    console.log('resPtr')
    console.log(resPtr)
    const res = resPtr.deref()
    return res
  }
}

module.exports = errno
