import type { BufferStruct } from 'ref-struct';
import type { TypeBase } from 'ref';
/**
 * some buffers used in this library have external pointer flag.
 */
export declare class BufferFFI extends Buffer {
    /**
     * the buffer have external pointer about node sandbox implementation,
     * if external field is true.
     */
    external?: boolean;
}
/**
 * type for foreign function call
 */
export interface TypeFFI extends BufferStruct {
    /**
     * type for foreign function call type.
     */
    ffi_type: BufferFFI;
}
/**
 * convert some object into TypeFFI
 */
export interface Type {
    (type: string | TypeFFI | TypeBase): TypeFFI;
    /**
     * primitive type equivalent pointer
     */
    pointerPrimitive: TypeFFI;
    /**
     * ffi type
     */
    FFI_TYPE: TypeFFI;
}
declare const Type: Type;
export { Type, Type as default };
