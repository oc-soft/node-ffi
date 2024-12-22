import { createRequire } from 'node:module';
const require = createRequire(import.meta.url);
const Library = require('./library.js');
const EXT = Library.EXT;
export { Library, Library as default };
// vi: se ts=2 sw=2 et:
