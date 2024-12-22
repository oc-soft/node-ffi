import { CIF } from 'ffi';
import * as ref from 'ref';
import * as assert from 'node:assert';
import { Buffer } from 'node:buffer';
export function run() {
    const cif = CIF(ref.types.void, []);
    assert.ok(Buffer.isBuffer(cif));
}
// vi: se ts=2 sw=2 et:
