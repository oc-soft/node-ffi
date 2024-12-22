import { createRequire } from 'node:module';
const require = createRequire(import.meta.url);
const errno = require('./errno.js');
export { errno, errno as default };
// vi: ts=2 sw=2 et:
