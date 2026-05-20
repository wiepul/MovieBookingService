#include "BookingService.h"
#include <algorithm>
#include <iostream>
#include <memory>

using namespace std;

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

    auto createShowing = [this](const std::string& movieId, const std::string& theaterId) {
        auto showing = std::make_unique<Showing>();
        for (int i = 1; i <= kSeatsPerShowing; i++) {
            showing->allSeats.push_back("a" + std::to_string(i));
        }
        showings_[movieId + "_" + theaterId] = std::move(showing);
    };

    for (const auto& m : movies_) {
        for (const auto& tId : movieToTheaters_[m.id]) {
            createShowing(m.id, tId);
        }
    }
}

std::vector<Movie> BookingService::listMovies() const {
    return movies_;
}   

std::vector<Theater> BookingService::listTheatersForMovie(std::string movieId) const {
    std::cout << "Listing theaters for movie: " << movieId << std::endl;
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
    auto key = movieId + "_" + theaterId;
    std::cout<<"Looking for showing with key: " << key << std::endl;

    auto it = showings_.find(key);

    if (it == showings_.end()) {
        return {};
    }

    const auto& sh = it->second;    
    
    std::vector<SeatId> available;
    std::cout<<"Available seats for showing " << key << ": ";
    for (const auto& seat : sh->allSeats) {
        if (sh->booked.find(seat) == sh->booked.end()) {
            available.push_back(seat);
            std::cout << seat << " ";
        }
    }
    std::cout << std::endl;
    return available;
}

BookingResult BookingService::bookSeats(std::string movieId,
                               std::string theaterId,
                               const std::vector<string>& seats) {
    return {};
}
