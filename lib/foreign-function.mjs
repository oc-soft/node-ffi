import { createRequire } from 'node:module';
const require = createRequire(import.meta.url);
const ForeignFunction = require('./foreign_function.js');
export { ForeignFunction, ForeignFunction as default };
// vi: se ts=2 sw=2 et:
