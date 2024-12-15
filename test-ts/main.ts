import * as Callback from './callback'
import * as Cif from './cif'
import * as DynamicLibrary from './dynamic-library'
import * as Errno from './errno'
import * as Function from './function'
import * as Library from './library'
import * as Types from './types'

(() => {
  Callback.run() 
  Cif.run()
  DynamicLibrary.run() 
  Errno.run()
  Function.run()
  Library.run()
  Types.run()
})()

// vi: se ts=2 sw=2 et:
