#include "BookingService.h"

BookingService::BookingService() {
    // Initialize with some dummy data
    movies_ = {
        {"m1", "Movie1"},
        {"m2", "Movie2"},
        {"m3", "Movie3"}
    };
    theaters_ = {
        {"t1", "Cinema One"},
        {"t2", "Cinema Two"},
        {"t3", "Cinema Three"}
    };
}

std::vector<Movie> BookingService::listMovies() const {
    return movies_;
}   

std::vector<Theater> BookingService::listTheatersForMovie(std::string movieId) const {


    

    return theaters_;
}

BookingResult BookingService::bookSeats(std::string movieId,
                               std::string theaterId,
                               const std::vector<string>& seats) {
    // Booking logic not implemented yet
}
