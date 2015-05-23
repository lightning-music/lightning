package lightning

import (
	"strings"
	"testing"
)

func TestReadNote(t *testing.T) {
	note, err := ReadNote(strings.NewReader(`{"sample":"foo.wav","number":61,"velocity":87}`))
	if err != nil {
		t.Fatal(err)
	}
	if note.Sample != "foo.wav" {
		t.Fatalf("note.Sample is %s", note.Sample)
	}
	if note.Number != 61 {
		t.Fatalf("note.Number is %d", note.Number)
	}
	if note.Velocity != 87 {
		t.Fatalf("note.Velocity is %d", note.Velocity)
	}
}
