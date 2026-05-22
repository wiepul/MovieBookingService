#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using MovieId = std::string;
using TheaterId = std::string;
using SeatId = std::string;

struct Movie
{
    std::string id;
    std::string title;
};

struct Theater
{
    std::string id;
    std::string name;
};

enum class BookingError
{
    None,
    EmptyRequest,
    UnknownShowing,
    DuplicateSeats,
    UnknownSeat,
    SeatAlreadyBooked,
};

struct BookingResult
{
    bool success = false;
    BookingError error = BookingError::None;
    std::string message;
    std::vector<SeatId> bookedSeats;
};

class BookingService
{
public:
    BookingService();

    BookingService(const BookingService &) = delete;
    BookingService &operator=(const BookingService &) = delete;
    BookingService(BookingService &&) = delete;
    BookingService &operator=(BookingService &&) = delete;

    // List all movies (sorted by id).
    std::vector<Movie> listMovies() const;

    // Theaters showing the given movie (sorted by id). Empty if unknown movie.
    std::vector<Theater> listTheatersForMovie(const MovieId &movieId) const;

    // Seats currently available for the given (movie, theater).
    // Empty if the showing does not exist or all seats are booked.
    std::vector<SeatId> listAvailableSeats(const MovieId &movieId,
                                           const TheaterId &theaterId) const;

    // Book a set of seats. Succeeds only if every requested seat is
    // currently free. Duplicates in the request and unknown seat ids fail the
    // request with no state change.
    BookingResult bookSeats(const MovieId &movieId,
                            const TheaterId &theaterId,
                            const std::vector<SeatId> &seats);

private:
    void seedDemoData();

    struct Showing
    {
        std::mutex mutex;
        std::unordered_set<SeatId> booked;
        std::vector<SeatId> allSeats;
    };

    std::unordered_map<std::string, std::unique_ptr<Showing>> showings_;

    std::vector<Movie> movies_;
    std::vector<Theater> theaters_;
    std::unordered_map<std::string, std::vector<TheaterId>> movieToTheaters_;

    static constexpr int kSeatsPerShowing = 20;
};
