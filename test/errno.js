
var assert = require('assert')
  , ref = require('ref')
  , ffi = require('../')
  , errno = ffi.errno

describe('errno()', function () {

  afterEach(gc)

  it('should be a object', function () {
    assert.equal('object', typeof errno)
  })

  it('should set the errno with out-of-range "strtoul" value', function () {
    var lib = process.platform == 'win32' ? 'ucrtbase' : 'libc'
    var strtoul = new ffi.Library(lib, {
      'strtoul': [ 'ulong', [ 'string', 'string', 'int' ] ]
    }).strtoul
    strtoul('1234567890123456789012345678901234567890', null, 0)
    assert.equal(34, errno.errno) // errno == ERANGE
    errno.errno = 0
    assert.equal(0, errno.errno) // errno == 0
  })

})
