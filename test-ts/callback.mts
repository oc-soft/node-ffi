import { Callback, ForeignFunction } from 'ffi'
import * as ref from 'ref'
import * as assert from 'node:assert'


export function run(): void {
  const funcPtr = Callback(ref.types.int, [ref.types.int], Math.abs)

  const func = ForeignFunction(funcPtr, ref.types.int, [ref.types.int])
  assert.equal(1234, func(-1234))
}

// vi: se ts=2 sw=2 et:
