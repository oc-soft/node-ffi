import { createRequire } from 'node:module';
const require = createRequire(import.meta.url);
const DynamicLibrary = require('./dynamic_library.js');
export { DynamicLibrary, DynamicLibrary as default };
// vi: se ts=2 sw=2 et:
