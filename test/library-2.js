const assert = require('assert')
const process = require('node:process')
const path = require('node:path')
const fs = require('node:fs')
const ref = require('ref')
const ffi = require('../')

function findLibraryPath(libFile) {
  let result 
  if (process.platform == 'darwin') {
    if (!libFile.endsWith(ffi.Library.EXT))  {
      libFile += ffi.Library.EXT
    }
    const searchPaths = [
      '/usr/lib',
      '/opt/local/lib',
      '/usr/local/lib'
    ]
    for (let i = 0; i < searchPaths.length; i++) {
      const libPath = path.join(searchPaths[i], libFile)
      if (fs.existsSync(libPath)) {
        result = libPath
        break
      }
    }
  } else {
    result = libFile
  }
  return result
}

describe('Library2', function () {
  it('should accept mode arguments', function() {
    const libName = process.platform == 'win32' ? 'msvcrt' : 'libm'
    const lib = ffi.Library(libName, { 
      'ceil': ['double', [ 'double' ]]
    }, undefined,
        ffi.DynamicLibrary.FLAGS.RTLD_NOW
        | ffi.DynamicLibrary.FLAGS.RTLD_GLOBAL
        | ffi.DynamicLibrary.FLAGS.RTLD_LOCAL)

    assert(typeof lib.ceil === 'function')
  })
  it('should find symbol for darwin', function() {
    if (process.platform == 'darwin') {
      const charType = ref.types.char
      const intType = ref.types.int
      const exCharPtr = ref.refType(charType, true)
      const charPtr = ref.refType(charType)
      const libPath = findLibraryPath('libintl')
      const lib = ffi.Library(libPath, { 
        'textdomain': [ exCharPtr, [ charPtr ]]
      })
      assert(typeof lib.textdomain === 'function')
    }
  })
})
