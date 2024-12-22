import type { TypeFFI, BufferFFI } from './type.mjs'
import type { TypeBase } from 'ref'
import { createRequire } from 'node:module'


export type Callback = (
  retType: string | TypeFFI | TypeBase,
  argTypes: Array<string | TypeFFI | TypeBase>,
  abiOrFunc: number | ((... args: any) => any),
  func?: (... args: any) => any) => BufferFFI
  
const require = createRequire(import.meta.url)

const Callback = require('./callback.js') as Callback 

export { Callback, Callback as default }

// vi: se ts=2 sw=2 et:
