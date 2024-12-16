import { ffiType } from '../'
import * as ref from 'ref'
import * as assert from 'node:assert'
import { Buffer } from 'node:buffer'

export function run(): void {
  const typeFFI = ffiType(ref.types.int)
  assert.ok(Buffer.isBuffer(typeFFI))
  console.log(ffiType.FFI_TYPE)
}


// vi: se ts=2 sw=2 et:
