# Gribouillot
A software to draw geometric figures on a map, with a layer system and powerful functionalities.


## Compile (macOS)

You will require the `qt5` toolkit to compile the project:

```sh
brew install qt5
```

Then inside the project:

```sh
qmake # Generate the Makefile
make -j 4 # Starts the compilation using 4 threads
```