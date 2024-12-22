import type { TypeFFI, BufferFFI } from './type.mjs';
import type { TypeBase } from 'ref';
export type Callback = (retType: string | TypeFFI | TypeBase, argTypes: Array<string | TypeFFI | TypeBase>, abiOrFunc: number | ((...args: any) => any), func?: (...args: any) => any) => BufferFFI;
declare const Callback: Callback;
export { Callback, Callback as default };
