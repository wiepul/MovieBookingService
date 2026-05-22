#include "BookingService.h"
#include <gtest/gtest.h>
#include <algorithm>
#include <thread>

static bool contains(const std::vector<SeatId> &v, const SeatId &s)
{
    return std::find(v.begin(), v.end(), s) != v.end();
}

TEST(ListMovies, ReturnsSeeded)
{
    BookingService svc;
    auto movies = svc.listMovies();
    EXPECT_EQ(movies.size(), 3);

    std::set<MovieId> ids;

    for (const auto &m : movies)
        ids.insert(m.id);

    EXPECT_EQ(ids, (std::set<MovieId>{"m1", "m2", "m3"}));
}

TEST(ListMovies, SortedById)
{
    BookingService svc;
    auto movies = svc.listMovies();

    EXPECT_EQ(movies.size(), 3);
    EXPECT_EQ(movies[0].id, "m1");
    EXPECT_EQ(movies[1].id, "m2");
    EXPECT_EQ(movies[2].id, "m3");
}

TEST(ListTheaters, KnownMovieSorted)
{
    BookingService svc;
    const auto theaters = svc.listTheatersForMovie("m1");
    ASSERT_EQ(theaters.size(), 2);
    EXPECT_EQ(theaters[0].id, "t1");
    EXPECT_EQ(theaters[1].id, "t2");
}

TEST(ListTheaters, UnknownMovie)
{
    BookingService svc;
    EXPECT_TRUE(svc.listTheatersForMovie("DOES_NOT_EXIST").empty());
}

TEST(ListSeats, AllAvailableInitially)
{
    BookingService svc;
    auto seats = svc.listAvailableSeats("m1", "t1");
    EXPECT_EQ(seats.size(), 20);
}

TEST(ListSeats, UnknownShowing)
{
    BookingService svc;
    EXPECT_TRUE(svc.listAvailableSeats("m1", "t3").empty());
    EXPECT_TRUE(svc.listAvailableSeats("NOPE", "t1").empty());
}

TEST(Book, SingleSeat)
{
    BookingService svc;
    const auto r = svc.bookSeats("m1", "t1", {"a1"});
    ASSERT_TRUE(r.success);
    EXPECT_EQ(r.error, BookingError::None);
    EXPECT_EQ(r.bookedSeats, std::vector<SeatId>{"a1"});
    EXPECT_FALSE(contains(svc.listAvailableSeats("m1", "t1"), "a1"));
}

TEST(Book, MultipleSeats)
{
    BookingService svc;
    auto r = svc.bookSeats("m1", "t1", {"a1", "a2", "a3"});
    ASSERT_TRUE(r.success);
    EXPECT_EQ(r.bookedSeats.size(), 3);
}

TEST(Book, EmptyRequestFails)
{
    BookingService svc;
    const auto r = svc.bookSeats("m1", "t1", {});
    EXPECT_FALSE(r.success);
    EXPECT_EQ(r.error, BookingError::EmptyRequest);
}

TEST(Book, UnknownShowingFails)
{
    BookingService svc;
    const auto r = svc.bookSeats("m1", "t3", {"a1"});
    EXPECT_FALSE(r.success);
    EXPECT_EQ(r.error, BookingError::UnknownShowing);
}

TEST(Book, InvalidSeatIdFails)
{
    BookingService svc;
    const auto r = svc.bookSeats("m1", "t1", {"a99"});
    EXPECT_FALSE(r.success);
    EXPECT_EQ(r.error, BookingError::UnknownSeat);

    EXPECT_EQ(svc.listAvailableSeats("m1", "t1").size(), 20);
}

TEST(Book, DuplicatesInRequestFail)
{
    BookingService svc;
    const auto r = svc.bookSeats("m1", "t1", {"a1", "a2", "a1"});
    EXPECT_FALSE(r.success);
    EXPECT_EQ(r.error, BookingError::DuplicateSeats);
}

TEST(Book, AlreadyBookedFails)
{
    BookingService svc;
    ASSERT_TRUE(svc.bookSeats("m1", "t1", {"a1"}).success);
    const auto r = svc.bookSeats("m1", "t1", {"a1"});
    EXPECT_FALSE(r.success);
    EXPECT_EQ(r.error, BookingError::SeatAlreadyBooked);
}

// The key correctness property: a conflict in any one seat must roll back
// the whole request.
TEST(Book, AllOrNothing)
{
    BookingService svc;
    ASSERT_TRUE(svc.bookSeats("m1", "t1", {"a2"}).success);

    auto res = svc.bookSeats("m1", "t1", {"a1", "a2", "a3"});
    EXPECT_FALSE(res.success);

    auto avail = svc.listAvailableSeats("m1", "t1");
    EXPECT_TRUE(contains(avail, "a1"));
    EXPECT_FALSE(contains(avail, "a2"));
    EXPECT_TRUE(contains(avail, "a3"));
}

TEST(Book, ShowingsAreIsolated)
{
    BookingService svc;
    ASSERT_TRUE(svc.bookSeats("m1", "t1", {"a1"}).success);
    // Same seat id in a different showing must still be free.
    EXPECT_TRUE(contains(svc.listAvailableSeats("m1", "t2"), "a1"));
}

// Concurrent booking tests

// 50 threads race for the same seat. Exactly one must win.
TEST(Book, ConcurrentBooking)
{
    BookingService svc;
    const int numThreads = 50;
    std::vector<std::thread> threads;
    std::vector<bool> results(numThreads);

    for (int i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(
            [&svc, &results, i]
            {
                results[i] = svc.bookSeats("m1", "t1", {"a1"}).success;
            });
    }

    for (auto &t : threads)
        t.join();

    int successCount = std::count(results.begin(), results.end(), true);
    EXPECT_EQ(successCount, 1);
}
