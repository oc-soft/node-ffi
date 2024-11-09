const assert = require('assert');
const ref = require('ref');
const ffi = require('../');

describe('types-2', function () {

  it('write collect type into buffer', function() {
    const buf = Buffer.alloc(ref.sizeof.pointer)
    const ffiType = ffi.ffiType('double')
    buf.writePointer(ffiType, 0, ffiType.external)
    const ffiType1 = buf.readPointer(0, ffiType.external)
    assert.equal(ffiType.length, ffiType1.length) 
  }) 
  it('must have pointerPrimitive property', function() {
    assert(ffi.ffiType.pointerPrimitive)
  })
})

// vi: se ts=2 sw=2 et:
