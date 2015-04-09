package lightning

import (
	"bytes"
	"encoding/json"
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

// DecodeNote json-decode a Note
func DecodeNote(b []byte) (*Note, error) {
	n := new(Note)
	err := json.Unmarshal(b, n)
	return n, err
}

// DecodeNote json-decode a Note from a string
func DecodeNoteString(s string) (*Note, error) {
	n := new(Note)
	err := json.Unmarshal(bytes.NewBufferString(s).Bytes(), n)
	return n, err
}

// EncodeNote json-encode a Note
func EncodeNote(n *Note) ([]byte, error) {
	return json.Marshal(n)
}
