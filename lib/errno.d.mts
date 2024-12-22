/**
 * errno object
 */
export interface Errno {
    /**
     * errno field
     */
    get errno(): number;
    set errno(no: number);
}
declare const errno: Errno;
export { errno, errno as default };
