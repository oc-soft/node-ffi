import type { TypeFFI, BufferFFI  } from './type'
import type { FunctionType } from './function-type'


export default class Function {
  
  /**
   * constructor
   * @param {string | TypeFFI} retType - return type
   * @param {Array<string | TypeFFI} argTypes - argument types
   * @param {number=} abi - aplication binary interface type
   */
  constructor(
    retType: string | TypeFFI,
    artTypes: Array<string | TypeFFI>,
    abi?: number) 

  /**
   * foreign function interface type which is wrapped by Buffer
   */
  ffi_type: BufferFFI
  /**
   * function binary size in native
   */
  size: number
  /**
   * memory layout alignment
   */
  alignment: number

  /**
   * indirection count in ref module protocol
   */
  indirection: number
  
  /**
   * javascript callback function to native pointer wrapped by Buffer
   * @param { (...args: any) => any } fn - javascript callback
   * @return { BufferFFI } pointer wrapped by buffer.
   */
  toPointer(fn: (... args: any)=>any): BufferFFI

  /**
   * convert foreign function pointer wrapped by Buffer to javascript callable.
   * @param { Buffer } buf - pointer wrapped by buffer
   * @return { FunctionType } - javascript callable
   */
  toFunction(buf: Buffer): FunctionType


  /**
   * get pointer wrapped by Buffer from buffer
   * @param { Buffer } buf - buffer contains function pointer
   * @param { number } offset - offset to read function pointer
   * @return { BufferFFI } return pointer wrapped by Buffer
   */
  get(buf: Buffer, offset: number): BufferFFI

  /**
   * get pointer wrapped by Buffer from buffer
   * @param { Buffer } buf - buffer contains function pointer
   * @param { number } offset - offset to read function pointer
   * @param { Buffer | ((...arg: any) => any) } return pointer wrapped by Buffer
   */
  set(buf: Buffer,
    offset: number,
    vaule: Buffer | ((...arg: any) => any)): void 
}


// vi: se ts=2 sw=2 et:
