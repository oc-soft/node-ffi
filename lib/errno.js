var DynamicLibrary = require('./dynamic_library')
  , ForeignFunction = require('./foreign_function')
  , bindings = require('./bindings')
  , ref = require('ref')
let errno

if (process.platform != 'win32') {
  errno = bindings.Errno
} else {
  errno = require('./errno-ucrt');
}

module.exports = errno
// vi: se ts=2 sw=2 et:
