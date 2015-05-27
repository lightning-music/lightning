// lightning is a simple package for playing audio files
// with JACK (https://jackaudio.org). It provides control over
// sample pitch and gain, as well as a way to export the
// output signal to a file.
package lightning

// #cgo CFLAGS: -Wall -O2
// #cgo LDFLAGS: -L. -llightning -lm -ljack -lsndfile -lpthread -lsamplerate -logg
// #include "lightning.h"
import "C"

import (
	"errors"
	"math"
)

// Engine provides methods for playing audio files with JACK
type Engine interface {
	// Connect JACK audio outputs
	Connect(ch1 string, ch2 string) error
	// PlaySample plays an audio sample
	PlaySample(file string, pitch float64, gain float64) error
	// PlayNote plays a note
	PlayNote(note *Note) error
	// ExportStart start exporting to an audio file
	ExportStart(file string) int
	// ExportStop stop the currently running export job if there is one
	ExportStop() int
}

// impl Engine implementation
type impl struct {
	handle C.Lightning
}

// Connect JACK outputs
func (this *impl) Connect(ch1 string, ch2 string) error {
	err := int(C.Lightning_connect_to(this.handle, C.CString(ch1), C.CString(ch2)))
	if err != 0 {
		return errors.New("could not connect to JACK sinks")
	} else {
		return nil
	}
}

// PlaySample play an audio sample
func (this *impl) PlaySample(file string, pitch float64, gain float64) error {
	err := C.Lightning_play_sample(
		this.handle, C.CString(file), C.pitch_t(pitch), C.gain_t(gain),
	)
	if err != 0 {
		return errors.New("could not play sample")
	} else {
		return nil
	}
}

// getPitch calculates the sample playback speed for a given midi note
func getPitch(note *Note) float64 {
	return float64(math.Pow(2.0, (float64(note.Number)-60.0)/12.0))
}

// PlayNote plays a note with a sample
func (this *impl) PlayNote(note *Note) error {
	pitch := getPitch(note)
	gain := float64(float64(note.Velocity) / 127.0)
	return this.PlaySample(note.Sample, pitch, gain)
}

// ExportStart starts exporting to an audio file
func (this *impl) ExportStart(file string) int {
	return int(C.Lightning_export_start(
		this.handle, C.CString(file),
	))
}

// ExportStop stops exporting to an audio file
func (this *impl) ExportStop() int {
	return int(C.Lightning_export_stop(this.handle))
}

// NewEngine initializes a new lightning engine
func NewEngine() Engine {
	instance := new(impl)
	instance.handle = C.Lightning_init()
	return instance
}
