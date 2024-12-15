import * as ref from 'ref'
import type { TypeFFI } from './type'
import type { FunctionType } from './function-type'


/**
 * create foreign function callback which is called from native function.
 * @param {Buffer} funcPtr - which is wrapped by Buffer
 * @param {string | Type} returnType - return type for callback.
 * @param {Array<string | TypeBase>} argTypes - argument types for callback.
 * @param {number} abi - appliction binary api type -- eg calling conversion
 * @return {FunctionType} callable function.
 */
export default function ForeignFunction(
  funcPtr: Buffer, 
  returnType: string | TypeFFI,
  argTypes: Array<string | TypeFFI>,
  abi?: number) : FunctionType


// vi: se ts=2 sw=2 et:
