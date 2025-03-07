const assert = require('assert')
const ffi = require('../')

describe('DynamicLibrary-2', function () {
  it('should get a symbol from libintl', function () {
    if (process.platform == 'darwin') {
      const dlh = ffi.DynamicLibrary(
        '/opt/local/lib/' +  'libintl' + ffi.LIB_EXT,
        ffi.DynamicLibrary.FLAGS.RTLD_NOW)
      const name = 'gettext'
      console.log(dlh)
      console.log(dlh.error())
      const symbol = dlh.get(name)
      assert.equal(name, symbol.name)
      console.log(symbol)
    }
  })
})

