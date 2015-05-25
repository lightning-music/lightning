package main

import "github.com/lightning/lightning"
import "log"
import "os"
import "time"

func main() {
	if len(os.Args) != 2 {
		log.Fatalf("usage: play-sample SAMPLE")
	}
	engine := lightning.NewEngine()
	err := engine.Connect("system:playback_1", "system:playback_2")
	if err != nil {
		log.Fatal(err)
	}
	note := lightning.NewNote(os.Args[1], 60, 120)
	err = engine.PlayNote(note)
	if err != nil {
		log.Fatal(err)
	}
	time.Sleep(5 * time.Second)
}
