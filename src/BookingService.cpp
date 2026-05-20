#include "BookingService.h"

BookingService::BookingService() {
    // Initialize with some dummy data
    movies = {
        {"m1", "The Matrix"},
        {"m2", "Inception"},
        {"m3", "Interstellar"}
    };
    theaters = {
        {"t1", "Cinema One"},
        {"t2", "Cinema Two"},
        {"t3", "Cinema Three"}
    };
}

std::vector<Movie> BookingService::listMovies() const {
    return movies;
}   

std::vector<Theater> BookingService::listTheatersForMovie(std::string movieId) const {
    // For now, return all theaters for any movie
    return theaters;
}

BookingResult BookingService::bookSeats(std::string movieId,
                               std::string theaterId,
                               const std::vector<string>& seats) {
    // Booking logic not implemented yet
}
