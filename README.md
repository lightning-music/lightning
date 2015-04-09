lightning is a go wrapper around a c library for playing back audio
samples. With it you can

* Play any file format supported by libsndfile
* Control over playback speed and gain

For go usage see the [godocs](http://godoc.org/github.com/lightning/lightning).

For the C api, see [lightning.h](http://github.com/lightning/lightning/blob/master/lightning.h).

Build
=====

Dependencies:

* jack
* libsndfile
* libsamplerate

On ubuntu-based systems you can do

```shell
# apt-get install libjack-dev libsndfile1-dev libsamplerate0-dev
```

Then 

```shell
# make
# make install
```

If you wish to contribute, see CONTRIBUTING.