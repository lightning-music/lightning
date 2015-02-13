# lightning

lightning is a simple application that makes it easy and fun
to create music on unix-like operating systems (tested mostly
on linux, a little on mac os x).

### Download

* [stable][4]

### Requirements

**You must have a [JACK][6] server running to use lightning. On linux this is usually very easy (see below). On mac, use [jackosx.com][http://jackosx.com].**

lightning is built around a server program called [lightningd][2]. See the lightningd page for more detailed info.

Once you have lightningd up and running, point your web browser to `localhost:3428`.

### Packaging

The [Makefile][5] in this repo contains the following targets which build binary packages:

* linux

[1]: https://github.com/lightning/liblightning
[2]: https://github.com/lightning/lightningd
[3]: https://github.com/lightning/www
[4]: http://sorahan.net/lightning
[5]: https://github.com/lightning/lightning/blob/master/Makefile
[6]: http://jackaudio.org