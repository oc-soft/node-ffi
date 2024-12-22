import { createRequire } from 'node:module';
const require = createRequire(import.meta.url);
const CIF = require('./cif.js');
export { CIF, CIF as default };
// vi: ts=2 sw=2 et:
