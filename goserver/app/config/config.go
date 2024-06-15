package config

import (
	"log"
	"os"

	"github.com/joho/godotenv"
)

var DatabaseURL string

func InitConfig() {
	err := godotenv.Load()
	if err != nil {
		log.Fatal("Error loading .env file")
	}

	DatabaseURL = os.Getenv("DATABASE_URL")
	if DatabaseURL == "" {
		log.Fatal("DATABASE_URL environment variable is required")
	}
}
