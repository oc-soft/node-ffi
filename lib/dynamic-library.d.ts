import type { Buffer } from 'node:buffer'

/**
 * dl library flags
 */
type RTLD_Flags = {
  RTLD_LAZY: number
  RTLD_NOW: number
  RTLD_LOCAL: number
  RTLD_GLOBAL: number
  RTLD_NOLOAD: number
  RTLD_NODELETE: number
  RTLD_FIRST: number
  RTLD_NEXT: Buffer
  RTLD_DEFAULT: Buffer
  RTLD_SELF: Buffer
  RTLD_MAIN_ONLY: Buffer
}


/**
 * dynamic library object
 */
export class DynamicLibrary {

  /**
   * dynamic library related flags
   */
  static FLAGS: RTLD_Flags
  /**
   * constructor
   * @param {string} path - dynamic library path
   * @param {number} mode - dynamic library opening mode
   */
  constructor(
    path: string,
    mode: number)


  /**
   * close dynamic library handle
   */
  close(): void

  /**
   * get exported symbol
   * @param {string} symbol - specify exported symbol in dynamic library handle.
   * @return {Buffer} - buffer wrapped symbol pointer.
   */
  get(symbol: string): Buffer

  /**
   * get last error number about dynamic library related function call.
   * @return {number} - last error number 
   */
  error(): number
}

// vi: se ts=2 sw=2 et:
