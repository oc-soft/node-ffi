import type { TypeBase } from 'ref'
import { Buffer } from 'node:buffer'

export default function CIF_var(
  rtype: string | TypeBase,
  types: Array<string | TypeBase>,
  numFixedArgs: number,
  api: number): Buffer

// vi: se ts=2 sw=2 et:
