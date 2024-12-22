import { ffiType } from 'ffi';
import * as ref from 'ref';
import * as assert from 'node:assert';
import { Buffer } from 'node:buffer';
export function run() {
    assert.equal(typeof ffiType, 'function');
    const typeFFI = ffiType(ref.types.int);
    assert.ok(Buffer.isBuffer(typeFFI));
}
// vi: se ts=2 sw=2 et:
