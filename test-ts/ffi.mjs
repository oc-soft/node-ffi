import * as ffi from 'ffi';
import * as assert from 'node:assert';
export function run() {
    assert.equal(typeof ffi.LIB_EXT, 'string');
    assert.equal(typeof ffi.FFI_TYPE, 'function');
}
// vi: ts=2 sw=2 et:
