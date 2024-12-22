import type { TypeFFI } from './type.mjs';
import type { TypeBase } from 'ref';
import type { FunctionType } from './function-type.mjs';
/**
 * create foreign function callback which is called from native function.
 * @param {Buffer} funcPtr - which is wrapped by Buffer
 * @param {string | TypeBase | TypeFFI } returnType - return type for callback.
 * @param {Array<string | TypeBase | TypeFFI >} argTypes - argument types for callback.
 * @param {number} abi - appliction binary api type -- eg calling conversion
 * @return {FunctionType} callable function.
 */
export type ForeignFunctionType = (funcPtr: Buffer, returnType: string | TypeFFI | TypeBase, argTypes: Array<string | TypeFFI | TypeBase>, abi?: number) => FunctionType;
declare const ForeignFunction: FunctionType;
export { ForeignFunction, ForeignFunction as default };
