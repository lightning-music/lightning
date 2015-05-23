package lightning

import (
	"encoding/json"
	"io"
)

// Note defines a MIDI-style note
type Note struct {
	Sample   string `json:"sample"`
	Number   int32  `json:"number"`
	Velocity int32  `json:"velocity"`
}

func NewNote(sample string, num, vel int32) *Note {
	return &Note{sample, num, vel}
}

// ReadNote reads a JSON-formatted Note from an io.Reader
func ReadNote(r io.Reader) (*Note, error) {
	dec, n := json.NewDecoder(r), new(Note)
	err := dec.Decode(n)
	return n, err
}
