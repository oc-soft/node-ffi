import { DynamicLibrary, LIB_EXT } from 'ffi';
import * as assert from 'node:assert';
import * as process from 'node:process';
export function run() {
    const lib = process.platform != 'win32' ? 'libc' : 'ucrtbase';
    const handle = new DynamicLibrary(lib + LIB_EXT);
    assert.ok(handle instanceof DynamicLibrary);
}
// vi: se ts=2 sw=2 et:
