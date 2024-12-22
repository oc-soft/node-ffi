import { createRequire } from 'node:module';
const require = createRequire(import.meta.url);
/**
 * some buffers used in this library have external pointer flag.
 */
export class BufferFFI extends Buffer {
    /**
     * the buffer have external pointer about node sandbox implementation,
     * if external field is true.
     */
    external;
}
const Type = require('./type.js');
export { Type, Type as default };
// vi: se ts=2 sw=2 et:
