import { CIF } from '../'
import * as ref from 'ref'
import * as assert from 'node:assert'
import { Buffer } from 'node:buffer'

export function run(): void {

  const cif = CIF(ref.types.void, [])
  assert.ok(Buffer.isBuffer(cif))
}

// vi: se ts=2 sw=2 et:
