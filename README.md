<p align="center">
<img src="https://raw.githubusercontent.com/Morphux/Graphic/master/logo/single_penguin.png" /><br />
</p>
<p align="center">
<img src="https://img.shields.io/badge/language-c-blue.svg" /> &nbsp;
<img src="https://img.shields.io/badge/license-Apache--2.0-yellow.svg" /> &nbsp;
<a href="https://travis-ci.org/Morphux/lib"><img src="https://travis-ci.org/Morphux/libmpm.svg?branch=master"/></a> &nbsp;
<a href="https://scan.coverity.com/projects/morphux-libmpm">
  <img alt="Coverity Scan Build Status"
       src="https://scan.coverity.com/projects/11577/badge.svg"/>
</a>&nbsp;
<a href="https://codecov.io/gh/Morphux/libmpm">
  <img src="https://codecov.io/gh/Morphux/libmpm/branch/master/graph/badge.svg" alt="Codecov" />
</a>
<br />
<h1 align="center" style="border:none">Morphux/libmpm</h1>
<h6 align="center">Morphux C Package Management Library</h6>
</p>
<p align="center">
<a href="#install">Install</a> • <a href="#test">Test</a> • <a href="#use">Use</a> • <a href="#documentation">Documentation</a>
</p>

# Install
### Clone the repository
```
git clone https://github.com/Morphux/libmpm.git
```

### Init
```
make init
```

### Compile
```
make
```

# Test
```
valgrind
```

### Basic Tests
```
make check
```

### Memory Tests
```
make -C tests valgrind
```

### Coverage
```
make coverage
```

# Use
Compile with the following options
```
gcc [...] -Llibmpm/ -lmpm -Ilibmpm/inc 
```
Of course, ```libmpm``` directory must exist.

In code:
```C
#include <libmpm.h>
```

# Documentation
### Dependencies
```
doxygen
```

Generate it:
```
make doc
```
