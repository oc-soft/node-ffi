import { Buffer } from 'node:buffer';
import type { TypeBase } from 'ref';
export type CIF = (returnType: TypeBase, types: Array<string | TypeBase>, abi?: number) => Buffer;
declare const CIF: CIF;
export { CIF, CIF as default };
