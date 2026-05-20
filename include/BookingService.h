
#include <string>
#include <vector>

using namespace std;

using MovieId   = std::string;
using TheaterId = std::string;
using SeatId    = std::string;

struct Movie {
    string     id;
    std::string title;
};

struct Theater {
    string   id;
    std::string name;
};

struct BookingResult {
    bool                 success;
    std::string          message;
    std::vector<SeatId>  bookedSeats;
};

class BookingService {
public:
    BookingService();

    // List all movies (sorted by id).
    std::vector<Movie> listMovies() const;

    // Theaters showing the given movie (sorted by id). Empty if unknown movie.
    std::vector<Theater> listTheatersForMovie(std::string movieId) const;

    // Seats currently available for the given (movie, theater).
    // Empty if the showing does not exist or all seats are booked.
    std::vector<SeatId> listAvailableSeats(std::string movieId,
                                           std::string theaterId) const;

    // Book a set of seats. Succeeds only if every requested seat is
    // currently free. Duplicates in the request and unknown seat ids fail the
    // request with no state change.
    BookingResult bookSeats(std::string movieId,
                            std::string theaterId,
                            const std::vector<SeatId>& seats);

private:
    std::vector<Movie> movies;
    std::vector<Theater> theaters;
};
