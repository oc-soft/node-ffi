const DynamicLibrary = require('./dynamic_library');
const ForeignFunction = require('./foreign_function');
const bindings = require('./bindings');
const ref = require('ref');

const int = ref.types.int
const intPtr = ref.refType(int)
const intPtrEx = ref.refType(int, true)
const ucrtBase = DynamicLibrary('ucrtbase');


const _errno = ucrtBase.get('_errno')
const errnoPtr = ForeignFunction(_errno, intPtrEx, [])

class Errno {

  get errno() {
    const resPtr = errnoPtr()
    const valueRef = ref.alloc(int)
    const valueRefAddress = ref.alloc(intPtr, valueRef)
    ref.copyMemory(valueRefAddress, resPtr, int.size)
    return ref.get(valueRef)
  }

  set errno(errorValue) {
    const resPtr = errnoPtr()
    const valueRef = ref.alloc(int, errorValue)
    const valueRefAddress = ref.alloc(intPtr, valueRef)
    ref.copyMemory(resPtr, valueRefAddress, int.size)
  }
}
module.exports = new Errno()

// vi: se ts=2 sw=2 et:
