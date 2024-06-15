package routes

import (
	"boquila/app/user"
	"net/http"

	"github.com/gorilla/mux"
	"gorm.io/gorm"
)

func SetupRoutes(r *mux.Router, db *gorm.DB) {
	r.HandleFunc("/register", user.RegisterHandler(db)).Methods("POST")
	r.HandleFunc("/get_website_name", user.GetWebsiteNameHandler()).Methods("GET")
	r.HandleFunc("/generate_challenge", user.GenerateChallengeHandler(db)).Methods("GET")

	// Middleware to handle CORS
	r.Use(mux.CORSMethodMiddleware(r))
	r.Use(corsMiddleware)
}

// corsMiddleware handles CORS headers
func corsMiddleware(next http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		w.Header().Set("Access-Control-Allow-Origin", "*")
		w.Header().Set("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
		w.Header().Set("Access-Control-Allow-Headers", "Content-Type")
		if r.Method == "OPTIONS" {
			w.WriteHeader(http.StatusOK)
			return
		}
		next.ServeHTTP(w, r)
	})
}
