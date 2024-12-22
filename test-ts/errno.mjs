import { errno } from 'ffi';
import * as assert from 'node:assert';
export function run() {
    assert.equal(typeof errno, 'object');
}
// vi: se ts=2 sw=2 et:
