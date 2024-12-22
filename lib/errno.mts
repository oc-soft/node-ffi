import { createRequire } from 'node:module'

/**
 * errno object
 */
export interface Errno {

  /**
   * errno field
   */
  get errno(): number
  set errno(no: number)
}

const require = createRequire(import.meta.url)

const errno = require('./errno.js') as Errno


export { errno, errno as default }



// vi: ts=2 sw=2 et:

