const DynamicLibrary = require('./dynamic_library');
const ForeignFunction = require('./foreign_function');
const bindings = require('./bindings');
const ref = require('ref');

const int = ref.types.int
const intPtr = ref.refType(int, true)
const ucrtBase = DynamicLibrary('ucrtbase');


const _errno = ucrtBase.get('_errno')
const errnoPtr = ForeignFunction(_errno, intPtr, [])

class Errno {

  get errno() {
    const resPtr = errnoPtr()
    return resPtr.deref().deref()
  }

  set errno(errorValue) {
    const resPtr = errnoPtr()
    const valueRef = ref.alloc(int, errorValue)
    const valueRefAddress = ref.alloc(intPtr, errorValue)
    ref.copyMemory(resPtr, valueRefAddress, int.size)
  }
}
module.exports = new Errno()

// vi: se ts=2 sw=2 et:
