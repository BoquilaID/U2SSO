package user

import (
	"boquila/app/models"
	"encoding/json"
	"math/rand"
	"net/http"
	"time"

	"gorm.io/gorm"
)

type UserData struct {
	WebsitePublicKey string `json:"website_public_key"`
	ChildPublicKey   string `json:"child_public_key"`
	IndexValue       int    `json:"index_value"`
}

func RegisterHandler(db *gorm.DB) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		var userData UserData
		if err := json.NewDecoder(r.Body).Decode(&userData); err != nil {
			http.Error(w, err.Error(), http.StatusBadRequest)
			return
		}

		user := models.User{
			WebsitePublicKey: userData.WebsitePublicKey,
			ChildPublicKey:   "dummy_child_public_key",
			IndexValue:       1,
		}

		db.Create(&user)

		w.Header().Set("Content-Type", "application/json")
		json.NewEncoder(w).Encode(map[string]string{
			"status":  "success",
			"message": "User registered successfully.",
		})
	}
}

func GetWebsiteNameHandler() http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		websiteName := r.Host
		json.NewEncoder(w).Encode(map[string]string{
			"website_name": websiteName,
		})
	}
}

func GenerateChallengeHandler(db *gorm.DB) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		rand.Seed(time.Now().UnixNano())
		challenge := rand.Intn(1e9)

		newChallenge := models.Challenge{
			Challenge: challenge,
		}

		db.Create(&newChallenge)

		w.Header().Set("Content-Type", "application/json")
		json.NewEncoder(w).Encode(map[string]int{
			"challenge": challenge,
		})
	}
}
