import type { TypeFFI } from './type.mjs';
import type { TypeBase } from 'ref';
/**
 * function option
 */
declare type FuncOption = {
    /**
     * application binary interface type
     */
    abi: number;
    /**
     * flag to call asynchronously
     */
    async?: boolean;
    /**
     * flag to call with variadic arguments
     */
    vargs?: boolean;
};
/**
 * library interface
 */
declare type FunctionParam = [
    TypeFFI | TypeBase | string,
    Array<TypeFFI | TypeBase | string>,
    FuncOption?
];
/**
 * foreign function parameters
 */
declare type FunctionParams = {
    [funcName: string]: FunctionParam;
};
/**
 * It creates library which has specified functions.
 * Or it appends some specified functions into spefied library.
 * @param {string} libFile - library path
 * @param {FunctionParams}  funcs - function parameter
 * @param {{[funcName: string]: (... args: any) => any}= } lib - library
 * @return library object which has some functions.
 */
export interface Library {
    (libFile: string, funcs: FunctionParams, lib?: {
        [funcName: string]: (...args: any) => any;
    }): {
        [funcName: string]: (...args: any) => any;
    };
    /**
     * os specific dynamic library extension
     */
    EXT: string;
}
declare const Library: Library;
export { Library, Library as default };
