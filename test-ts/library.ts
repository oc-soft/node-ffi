import { Library } from '../'
import * as assert from 'node:assert'
import * as process from 'node:process'

export function run(): void {
  const lib = process.platform != 'win32' ? 'libm' : 'ucrtbase'
  const libm = Library(lib, {
      'ceil': [ 'double', [ 'double' ] ]
  })
  assert.equal(typeof libm.ceil, 'function');
  assert.equal(libm.ceil(1.1),  2);
}


// vi: se ts=2 sw=2 et:
