# lightning

lightning is a simple application that makes it easy and fun
to create music on unix-like operating systems (tested mostly
on linux, a little on mac os x).

## install from source

### requirements

#### liblightning

First, you have to build and install [liblightning][1].
This is a C library that plays audio files with [JACK][http://jackaudio.org].

#### lightningd

Next you have to build the [lightningd][2] server program.
This program provides a sample-sequencer that is controlled
via a websocket API.

#### www

Finally you have to install the [web components][3] for the lightning
front end. When you start lightningd, it must be told where the
web components have been installed with the `-www` flag.

### usage

Start lightningd.

```shell
$ lightningd -www /usr/share/lightning/www
```

Point your web browser to `localhost:3428`.

TODO: in-depth user docs.

[1]: https://github.com/lightning/liblightning
[2]: https://github.com/lightning/lightningd
[3]: https://github.com/lightning/www
