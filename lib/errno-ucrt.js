const DynamicLibrary = require('./dynamic_library');
const ForeignFunction = require('./foreign_function');
const bindings = require('./bindings');
const ref = require('ref');

const int = ref.types.int
const intPtr = ref.refType(int, true)
const ucrtBase = DynamicLibrary('ucrtbase.dll');


const _errno = ucrtBase.get('_errno')
const errnoPtr = ForeignFunction(_errno, intPtr, [])

class Errno {

  get errno() {
    console.log('intPtr')
    console.log(intPtr)
    const resPtr = errnoPtr()
    console.log('resPtr')
    console.log(resPtr)
    const resRef = resPtr.deref()
    console.log(resRef)
    return resRef
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
