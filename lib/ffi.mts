
import { createRequire } from 'node:module'
import type { Types } from 'ref'

import { VariadicForeignFunction } from './variadic-foreign-function.mjs'
import { CIF_var } from './cif-var.mjs'
import { CIF } from './cif.mjs'
import { Type as ffiType } from './type.mjs'

import { Function } from './function.mjs'
import { ForeignFunction } from './foreign-function.mjs'
import { DynamicLibrary } from './dynamic-library.mjs'
import { Library,  } from './library.mjs'
import { Callback } from './callback.mjs'
import { errno } from './errno.mjs'

const require = createRequire(import.meta.url)

const ffi = require('./ffi.js')


const types = ffi.types as Types
const version = ffi.version as string
const FFI_TYPE = ffiType.FFI_TYPE
const LIB_EXT = Library.EXT

export {
  types, 
  version, 
  CIF,
  CIF_var,
  LIB_EXT,
  FFI_TYPE,
  Function,
  ForeignFunction,
  DynamicLibrary,
  Library,
  Callback,
  errno,
  ffiType
}

// vi: se ts=2 sw=2 et:
