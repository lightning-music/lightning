lightning is a go wrapper around a c library for playing back audio
samples. With it you can

* Play any file format supported by libsndfile
* Control over playback speed and gain

Install
=======

First, install the following dependencies:

* jack
* libsndfile
* libsamplerate

#### C

Installing as a C library

```shell
make
make install
```

On ubuntu-based systems you can do

```shell
apt-get install libjack-dev libsndfile1-dev libsamplerate0-dev
```

To install the go library you have to install the dependencies,
install [go](http://golang.org/dl), follow [these](http://golang.org/doc/code.html)
instructions for setting up your workspace and GOPATH environment variable, then

```shell
go get github.com/lightning/lightning
```

Usage
=====

For go usage see the [godocs](http://godoc.org/github.com/lightning/lightning).

For the C api, see [lightning.h](http://github.com/lightning/lightning/blob/master/lightning.h).

Contributing
============

If you wish to contribute, see [CONTRIBUTING](http://github.com/lightning/lightning/blob/master/CONTRIBUTING).
