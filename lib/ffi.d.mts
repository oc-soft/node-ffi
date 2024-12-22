import type { Types } from 'ref';
import { CIF_var } from './cif-var.mjs';
import { CIF } from './cif.mjs';
import { Type as ffiType } from './type.mjs';
import { Function } from './function.mjs';
import { ForeignFunction } from './foreign-function.mjs';
import { DynamicLibrary } from './dynamic-library.mjs';
import { Library } from './library.mjs';
import { Callback } from './callback.mjs';
import { errno } from './errno.mjs';
declare const types: Types;
declare const version: string;
declare const FFI_TYPE: import("./type.mjs").TypeFFI;
declare const LIB_EXT: string;
export { types, version, CIF, CIF_var, LIB_EXT, FFI_TYPE, Function, ForeignFunction, DynamicLibrary, Library, Callback, errno, ffiType };
