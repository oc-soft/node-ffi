import { createRequire } from 'node:module';
const require = createRequire(import.meta.url);
const Function = require('./function.js');
export { Function, Function as default };
// vi: se ts=2 sw=2 et:
