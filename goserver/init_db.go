package main

import (
	"log"
	"os"

	"boquila/app/models"

	"gorm.io/driver/sqlite"
	"gorm.io/gorm"
)

func main() {
	dbPath := "app.db"

	// Remove existing database file if it exists
	if _, err := os.Stat(dbPath); err == nil {
		os.Remove(dbPath)
		log.Println("Existing database file removed.")
	}

	// Create new database
	db, err := gorm.Open(sqlite.Open(dbPath), &gorm.Config{})
	if err != nil {
		log.Fatalf("failed to connect to the database: %v", err)
	}

	// Auto-migrate database models
	db.AutoMigrate(&models.User{}, &models.Challenge{})
	log.Println("Database tables created.")
}
