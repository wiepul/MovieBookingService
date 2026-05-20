#include "BookingService.h"
#include <algorithm>

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

    movieToTheaters_ = {
        {"m1", {"t1", "t2"}},
        {"m2", {"t2"}},
        {"m3", {"t3"}}
    };
}

std::vector<Movie> BookingService::listMovies() const {
    return movies_;
}   

std::vector<Theater> BookingService::listTheatersForMovie(std::string movieId) const { 
    auto it = movieToTheaters_.find(movieId);
    if (it == movieToTheaters_.end()) {
        return {};
    }
    
    std::vector<Theater> result;
    for (const auto& theaterId : it->second) {
        auto theaterIt = find_if(theaters_.begin(), theaters_.end(), [&](const Theater& t) {
            return t.id == theaterId;
        });
        if (theaterIt != theaters_.end()) {
            result.push_back(*theaterIt);
        }
    }
    return result;
}

std::vector<SeatId> BookingService::listAvailableSeats(std::string movieId,
                                   std::string theaterId) const {
return {};
                                }

BookingResult BookingService::bookSeats(std::string movieId,
                               std::string theaterId,
                               const std::vector<string>& seats) {
    return {};
}
