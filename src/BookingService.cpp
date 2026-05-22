#include "BookingService.h"

#include <algorithm>
#include <memory>

namespace
{

// Converts internal error codes into user-facing messages.
std::string bookingErrorMessage(BookingError error, const SeatId &seat = {})
{
    switch (error)
    {
    case BookingError::None:
        return "Seats booked successfully.";
    case BookingError::EmptyRequest:
        return "No seats requested.";
    case BookingError::UnknownShowing:
        return "Showing does not exist.";
    case BookingError::DuplicateSeats:
        return "Duplicate seat IDs in request.";
    case BookingError::UnknownSeat:
        return "Unknown seat ID: " + seat;
    case BookingError::SeatAlreadyBooked:
        return "One of the requested seats is already booked: " + seat;
    }
    return "Booking failed.";
}

BookingResult makeFailure(BookingError error, const SeatId &seat = {})
{
    return BookingResult{false, error, bookingErrorMessage(error, seat), {}};
}

} // namespace

void BookingService::seedDemoData()
{
    // Seed a small in-memory catalog used by the demo CLI and tests.
    movies_ = {
        {"m1", "Movie1"},
        {"m2", "Movie2"},
        {"m3", "Movie3"}};
    theaters_ = {
        {"t1", "Cinema One"},
        {"t2", "Cinema Two"},
        {"t3", "Cinema Three"}};

    movieToTheaters_ = {
        {"m1", {"t1", "t2"}},
        {"m2", {"t2"}},
        {"m3", {"t3"}}};

    std::sort(movies_.begin(), movies_.end(),
              [](const Movie &a, const Movie &b) { return a.id < b.id; });
    std::sort(theaters_.begin(), theaters_.end(),
              [](const Theater &a, const Theater &b) { return a.id < b.id; });

    auto createShowing = [this](const std::string &movieId, const std::string &theaterId)
    {
        auto showing = std::make_unique<Showing>();
        // Create deterministic seat IDs: a1..a20.
        for (int i = 1; i <= kSeatsPerShowing; ++i)
        {
            showing->allSeats.push_back("a" + std::to_string(i));
        }
        showings_[movieId + "_" + theaterId] = std::move(showing);
    };

    for (const auto &movie : movies_)
    {
        for (const auto &theaterId : movieToTheaters_[movie.id])
        {
            createShowing(movie.id, theaterId);
        }
    }
}

BookingService::BookingService()
{
    // Build initial in-memory state at startup.
    seedDemoData();
}

std::vector<Movie> BookingService::listMovies() const
{
    return movies_;
}

std::vector<Theater> BookingService::listTheatersForMovie(const MovieId &movieId) const
{
    // Resolve theater IDs for the movie, then map them to theater objects.
    const auto it = movieToTheaters_.find(movieId);
    if (it == movieToTheaters_.end())
    {
        return {};
    }

    std::vector<Theater> result;
    for (const auto &theaterId : it->second)
    {
        const auto theaterIt = std::find_if(
            theaters_.begin(), theaters_.end(),
            [&](const Theater &t) { return t.id == theaterId; });
        if (theaterIt != theaters_.end())
        {
            result.push_back(*theaterIt);
        }
    }

    // Return a stable order to simplify test assertions & readability.
    std::sort(result.begin(), result.end(),
              [](const Theater &a, const Theater &b) { return a.id < b.id; });
    return result;
}

std::vector<SeatId> BookingService::listAvailableSeats(const MovieId &movieId,
                                                       const TheaterId &theaterId) const
{
    const auto key = movieId + "_" + theaterId;
    const auto it = showings_.find(key);
    if (it == showings_.end())
    {
        return {};
    }

    const auto &showing = it->second;
    // Lock per-showing so reads are consistent with concurrent bookings.
    std::lock_guard<std::mutex> lock(showing->mutex);

    std::vector<SeatId> available;
    for (const auto &seat : showing->allSeats)
    {
        if (showing->booked.find(seat) == showing->booked.end())
        {
            available.push_back(seat);
        }
    }
    return available;
}

BookingResult BookingService::bookSeats(const MovieId &movieId,
                                        const TheaterId &theaterId,
                                        const std::vector<SeatId> &seats)
{
    // Return early on invalid request shape before touching shared state.
    if (seats.empty())
    {
        return makeFailure(BookingError::EmptyRequest);
    }

    const auto key = movieId + "_" + theaterId;
    const auto it = showings_.find(key);
    if (it == showings_.end())
    {
        return makeFailure(BookingError::UnknownShowing);
    }

    const std::unordered_set<SeatId> requestedSeats(seats.begin(), seats.end());
    if (requestedSeats.size() != seats.size())
    {
        return makeFailure(BookingError::DuplicateSeats);
    }

    auto &showing = it->second;
    // Validate under one lock.
    std::lock_guard<std::mutex> lock(showing->mutex);

    // Validate every seat in the request.
    for (const auto &seat : seats)
    {
        if (std::find(showing->allSeats.begin(), showing->allSeats.end(), seat) ==
            showing->allSeats.end())
        {
            return makeFailure(BookingError::UnknownSeat, seat);
        }

        if (showing->booked.find(seat) != showing->booked.end())
        {
            return makeFailure(BookingError::SeatAlreadyBooked, seat);
        }
    }

    // Commit only after the full request is validated.
    for (const auto &seat : seats)
    {
        showing->booked.insert(seat);
    }

    return BookingResult{true, BookingError::None, bookingErrorMessage(BookingError::None),
                         seats};
}
