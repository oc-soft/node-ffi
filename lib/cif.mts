import { createRequire } from 'node:module'
import { Buffer } from 'node:buffer'
import type { TypeBase } from 'ref'

export type CIF = (returnType: TypeBase,
  types: Array<string | TypeBase>,
  abi?: number) => Buffer

const require = createRequire(import.meta.url)

const CIF = require('./cif.js') as CIF

export { CIF, CIF as default }


// vi: ts=2 sw=2 et:
