import { createRequire } from 'node:module'
import type { TypeBase } from 'ref'
import { Buffer } from 'node:buffer'

export type CIFVariadicFunction = (
  rtype: string | TypeBase,
  types: Array<string | TypeBase>,
  numFixedArgs: number,
  api: number) => Buffer



const require = createRequire(import.meta.url)

const CIF_var = require('./cif_var.js') as CIFVariadicFunction

export { CIF_var, CIF_var as default }

// vi: se ts=2 sw=2 et:
