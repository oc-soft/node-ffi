import { Buffer } from 'node:buffer'
import type { TypeBase } from 'ref'

export default function CIF(returnType: TypeBase,
  types: Array<string | TypeBase>,
  abi?: number): Buffer

// vi: se ts=2 sw=2 et:
