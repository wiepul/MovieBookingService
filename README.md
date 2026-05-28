# Movie Booking Service

A small C++14 backend for booking movie seats, exposed as a static library (`booking`) and embedded in a simple CLI front-end (`movie_booking_cli`).

>The project builds with CMake and has a GoogleTest-based test suite (`booking_tests`). Tests are fetched via CMake FetchContent (GoogleTest v1.14.0).

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
├── Dockerfile
├── .dockerignore
├── include/BookingService.h
├── src/
│   ├── BookingService.cpp
│   └── main.cpp
└── tests/
    ├── CMakeLists.txt
    └── BookingServiceTests.cpp
```

## Requirements

- A C++14 compiler (MSVC 2017+, GCC 7+, Clang 5+)
- CMake 3.14+
- Internet access on first build to fetch GoogleTest
- Pass `-DBUILD_TESTING=OFF` to skip test building

## Build & run

### Windows

Option 1: from a Visual Studio Developer Command Prompt with MSVC installed:

```cmd
cmake -S . -B build
cmake --build build --config Release
build\bin\movie_booking_cli.exe
```

Tests (build first, then run from `build`):

```cmd
cmake --build build --config Release
cd build
ctest -C Release --output-on-failure
```

Option 2: from an MSYS2 / MinGW shell or when using GCC/Clang on Windows:

```bash
cmake -G "MinGW Makefiles" -S . -B build
cmake --build build
./build/bin/movie_booking_cli.exe
```

Tests:

```bash
cmake --build build
cd build
ctest --output-on-failure
```

Run tests directly: `./build/bin/booking_tests.exe`

### Linux / macOS

```bash
cmake -S . -B build
cmake --build build -j
./build/bin/movie_booking_cli
```

Tests:

```bash
cmake --build build -j
cd build && ctest --output-on-failure
```

Run tests directly: `./build/bin/booking_tests`

### Skipping tests

```bash
cmake -S . -B build -DBUILD_TESTING=OFF
```

## Docker (optional)

Initial Docker support is included (`Dockerfile`, `.dockerignore`). It is optional — the normal CMake workflow above still works without Docker.

Requires Docker Engine (Linux) or Docker Desktop (Windows/macOS).

```bash
docker build -t movie-booking-service .
docker run --rm -it movie-booking-service
docker build --target test -t movie-booking-service:test .
```

This is a basic setup and may fail depending on your environment (for example missing runtime libraries or network access during the GoogleTest fetch). Additional adaptation of the `Dockerfile` may be required on your machine.

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

=== Movie Booking ===
 ...
> 4
Movie id: m1
Theater id: t1
Seats (e.g. a1,a2): a1,a2,a3
Booked: a1 a2 a3
```

On failure, the CLI prints `Failed: <message>` using `BookingResult::message` (for example, unknown showing or seat already booked).

## API

Defined in `include/BookingService.h`.

**Types:** `MovieId`, `TheaterId`, and `SeatId` are `std::string` aliases. `Movie` and `Theater` hold `id` and `title`/`name`. `BookingResult` contains `success`, `error` (`BookingError`), `message`, and `bookedSeats`.

**BookingError values:** `None`, `EmptyRequest`, `UnknownShowing`, `DuplicateSeats`, `UnknownSeat`, `SeatAlreadyBooked`.

`BookingService` is non-copyable and non-movable (copy and move operations are deleted).

```cpp
class BookingService {
public:
    BookingService();

    std::vector<Movie> listMovies() const;
    std::vector<Theater> listTheatersForMovie(const MovieId& movieId) const;
    std::vector<SeatId> listAvailableSeats(const MovieId& movieId,
                                           const TheaterId& theaterId) const;
    BookingResult bookSeats(const MovieId& movieId,
                            const TheaterId& theaterId,
                            const std::vector<SeatId>& seats);
};
```

