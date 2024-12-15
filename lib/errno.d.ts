
/**
 * errno object
 */
declare class Errno {

  /**
   * errno field
   */
  get errno(): number
  set errno(no: number)
}

declare const errnoObj: Errno

export default errnoObj

// vi: se ts=2 sw=2 et:
