import type { TypeBase } from 'ref';
import { Buffer } from 'node:buffer';
export type CIFVariadicFunction = (rtype: string | TypeBase, types: Array<string | TypeBase>, numFixedArgs: number, api: number) => Buffer;
declare const CIF_var: CIFVariadicFunction;
export { CIF_var, CIF_var as default };
