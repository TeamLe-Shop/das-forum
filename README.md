# das-forum

A server for communicating with other users.

## Libraries

The used libraries are:
  - types (http://github.com/TeamLe-Shop/types) (Provided in `lib/`)
  - jansson, a C library for manipulating JSON data
    (https://github.com/akheron/jansson) (Must install manually)

## Building

The build process is as follows:
  1. Run `make install-dependencies` if the dependencies have not been installed
     yet on your system. This is not required for subsequent builds.
  2. Run `make` to build the source.

## Running

DasForum™ by default listens on port 7778. You can create a `config.json`
relative to the executable and specify the port there. For example:

```json
{
    "port": 9001
}
```

You can specify the path to the configuration file using the command line
argument `-config`.

```sh
$ ./dasforum -config verygoodconfigname.json
```
