import type {BufferFFI} from './type'

export const FFI_SARG_SIZE: number
export const FFI_TYPE_SIZE: number
export const FFI_CIF_SIZE: number

export const FFI_OK: number
export const FFI_BAD_TYPEDEF: number
export const FFI_BAD_ABI

// `ffi_abi` enum values
export const FFI_DEFAULT_ABI: number
export const FFI_FIRST_ABI: number
export const FFI_LAST_ABI: number
export const FFI_SYSV: number
/* ---- ARM processors ---------- */
export const FFI_VFP: number
/* ---- Intel x86 Win32 ---------- */
export const FFI_STDCALL: number
export const FFI_THISCALL: number
export const FFI_FASTCALL: number
export const FFI_MS_CDECL: number
export const FFI_WIN64: number
export const FFI_UNIX64: number

export const HAS_OBJC: boolean

export const RTLD_LAZY: number
export const RTLD_NOW: number
export const RTLD_LOCAL: number
export const RTLD_GLOBAL: number
export const RTLD_NOLOAD: number
export const RTLD_NODELETE: number
export const RTLD_FIRST: number
export const RTLD_NEXT: BufferFFI
export const RTLD_DEFAULT: BufferFFI
export const RTLD_SELF: BufferFFI
export const RTLD_MAIN_ONLY: BufferFFI

/**
 * foreign function interface version
 */
export const version: string

// vi: se ts=2 sw=2 et:

