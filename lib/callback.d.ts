import type { TypeFFI, BufferFFI } from './type'


export default function Callback(
  retType: string | TypeFFI,
  argTypes: Array<string | TypeFFI>,
  abiOrFunc: number | ((... args: any) => any),
  func?: (... args: any) => any): BufferFFI
  


// vi: se ts=2 sw=2 et:
