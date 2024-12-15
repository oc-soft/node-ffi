import type { TypeFFI, BufferFFI } from './type'
import type { TypeBase } from 'ref'


export default function Callback(
  retType: string | TypeFFI | TypeBase,
  argTypes: Array<string | TypeFFI | TypeBase>,
  abiOrFunc: number | ((... args: any) => any),
  func?: (... args: any) => any): BufferFFI
  


// vi: se ts=2 sw=2 et:
