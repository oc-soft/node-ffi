
export * from './constants'
export { default as CIF } from './cif'
export { default as CIF_var } from './cif-var'
export { default as Function} from './function'
export { default as ForeignFunction } from './foreign-function'
export { default as VariadicForeignFunction } from './variadic-foreign-function'
export { default as DynamicLibrary } from './dynamic-library'
export { default as Callback } from './callback'
export { default as errno } from './errno'
export { default as ffi_type } from './type'
export { default as Library, EXT as LIB_EXT } from './library'

const FFI_TYPE = ffi_type.FFI_TYPE
export FFI_TYPE

// vi: se ts=2 sw=2 et:
