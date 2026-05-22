# Movie Booking Service

A small C++14 backend for booking movie seats, exposed as a static library and embedded in a simple CLI front-end.

> The project builds with CMake and has a GoogleTest-based test suite. The tests are fetched via CMake FetchContent.

## What it does

- List all seeded movies
- List theaters showing a given movie
- List available seats for a specific movie/theater showing
- Book one or more seats atomically
- Prevent double-booking with a per-showing lock

## Layout

```
MovieBookingService/
├── CMakeLists.txt
├── include/BookingService.h
├── src/
│   ├── BookingService.cpp
│   └── main.cpp
└── tests/
    ├── CMakeLists.txt
    └── BookingServiceTests.cpp
```

## Requirements

- A C++14 compiler (MSVC 2017+, GCC 5+, Clang 3.8+)
- CMake 3.10+
- Internet access on first build to fetch GoogleTest
- Pass `-DBUILD_TESTING=OFF` to skip test building

## Build & run

### Windows

From a Developer Command Prompt:

```cmd
mkdir build
cmake -S . -B build
cmake --build build --config Release
build\bin\movie_booking_cli.exe
```

Tests:

```cmd
mkdir build
cd build
ctest -C Release --output-on-failure
```

### Linux / macOS

```bash
cmake -S . -B build
cmake --build build -j
./build/bin/movie_booking_cli
```

Tests:

```bash
cd build && ctest --output-on-failure
```

### Skipping tests

```bash
cmake -S . -B build -DBUILD_TESTING=OFF
```

## Example session

```
Movie Booking CLI. Pick an option from the menu.

=== Movie Booking ===
 1. List movies
 2. List theaters showing a movie
 3. List available seats
 4. Book seats
 5. Exit
> 1
Movies:
 - m1: Movie1
 - m2: Movie2
 - m3: Movie3
> 4
Movie id: m1
Theater id: t1
Seats (e.g. a1,a2): a1,a2,a3
Booked: a1 a2 a3
```

## API

Defined in `include/BookingService.h`. The public surface is:

```cpp
class BookingService {
public:
    BookingService();

    std::vector<Movie> listMovies() const;
    std::vector<Theater> listTheatersForMovie(std::string movieId) const;
    std::vector<SeatId> listAvailableSeats(std::string movieId,
                                           std::string theaterId) const;
    BookingResult bookSeats(std::string movieId,
                            std::string theaterId,
                            const std::vector<SeatId>& seats);
};
```

All methods are safe to call concurrently from multiple threads.

## Design notes

- **In-memory only.** No persistence, no database.
- **Per-showing locking.** Each `(movieId, theaterId)` showing has its own mutex.
- **All-or-nothing booking.** If any requested seat is invalid or already taken, the request fails with no state change.
- **Seeded demo data.** The service is initialized with three movies, three theaters, and 20 seats per showing (`a1`..`a20`).

## Tests

The suite covers:

- Listing movies, theaters, and seats
- Unknown movie/showing handling
- Single and multi-seat bookings
- Empty requests, duplicate seat ids, invalid seat ids, already-booked seats
- All-or-nothing booking semantics
- Concurrent booking of the same seat with many threads