package main

import (
	"fmt"
	"io/ioutil"
	"time"

	"./tinysegmenter"
)

func main() {
	b, err := ioutil.ReadFile("./timemachineu8j.txt")
	if err != nil {
		panic(err)
	}
	sampletext := string(b)

	s := tinysegmenter.NewSegmenter()

	start := time.Now()
	for i := 0; i < 100; i++ {
		s.Segment(sampletext)
	}
	end := time.Now()
	fmt.Println(end.Sub(start))
}
