# cqf
Counting Quotient Filter Library

# Compiling and Testing

We're using the [Catch2 library](https://github.com/catchorg/Catch2) as our testing framework.

From source folder

`git submodule init`\
`git submodule update`\
`cmake . -B build`\
`cd build/apps && make && cd ../cqf && make`

Run the `CQF` binary present at build/apps to run your main.
Run the `runTests` binary present at build/cqf to run your tests.

# Documentation
 
Doxygen style documentation.
`doxygen Doxyfile`

In GNOME:
`xdg-open /docs/html/index.html`
