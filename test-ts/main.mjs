import * as Callback from './callback.mjs';
import * as Cif from './cif.mjs';
import * as DynamicLibrary from './dynamic-library.mjs';
import * as Errno from './errno.mjs';
import * as Function from './function.mjs';
import * as Library from './library.mjs';
import * as Types from './types.mjs';
import * as ffi from './ffi.mjs';
(() => {
    Callback.run();
    Cif.run();
    DynamicLibrary.run();
    Errno.run();
    Function.run();
    Library.run();
    Types.run();
    ffi.run();
})();
// vi: se ts=2 sw=2 et:
