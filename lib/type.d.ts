import type { TypeBase } from 'ref'
import type { BufferStruct } from 'ref-struct'
import type { Buffer } from 'node:buffer'

/**
 * some buffers used in this library have external pointer flag.
 */
export class BufferFFI extends Buffer {
  /**
   * the buffer have external pointer about node sandbox implementation, 
   * if external field is true.
   */
  external? : boolean
}

/**
 * type for foreign function call
 */
export interface TypeFFI extends BufferStruct {
  /**
   * type for foreign function call type.
   */
  ffi_type: BufferFFI
}


/**
 * convert some object into TypeFFI
 */
export function Type(type: string | TypeFFI | TypeBase): TypeFFI

export namespace Type {
  /**
   * primitive type equivalent pointer
   */
  export const pointerPrimitive: TypeFFI

  /**
   * ffi type
   */
  export const FFI_TYPE: TypeFFI
}


export default Type 

// vi: se ts=2 =sw=2 et:
