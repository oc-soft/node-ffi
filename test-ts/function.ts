import { Function } from '../'
import * as assert from 'node:assert'


export function run():void {

  const voidFn = new Function('void', [])
    
  assert.equal(typeof voidFn.get, 'function')
  assert.equal(typeof voidFn.set, 'function')
}

// vi: se ts=2 sw=2 et:
