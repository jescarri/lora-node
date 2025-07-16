package main

import (
	"context"
	"log"
	"os"

	"github.com/urfave/cli/v3"
	"lora-sensor-cli/cmd"
)

func main() {
	app := &cli.Command{
		Name:  "lora-sensor-cli",
		Usage: "CLI tool for managing LoRa sensor firmware updates",
		Commands: []*cli.Command{
			cmd.NewUpdateDeviceCommand(),
		},
	}

	if err := app.Run(context.Background(), os.Args); err != nil {
		log.Fatal(err)
	}
}
