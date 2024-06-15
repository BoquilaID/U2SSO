package models

import (
	"time"
)

type User struct {
	ID               uint `gorm:"primaryKey"`
	WebsitePublicKey string
	ChildPublicKey   string
	IndexValue       int
}

type Challenge struct {
	ID        uint `gorm:"primaryKey"`
	Challenge int
	CreatedAt time.Time
}
