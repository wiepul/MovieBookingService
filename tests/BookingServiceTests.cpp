#include "BookingService.h"
#include <gtest/gtest.h>

static bool contains(const std::vector<SeatId>& v, const SeatId& s) {
    return std::find(v.begin(), v.end(), s) != v.end();
}

TEST(ListMovies, ReturnsSeeded) {
    BookingService svc;
    auto movies = svc.listMovies();
    EXPECT_EQ(movies.size(), 3);

    std::set<MovieId> ids;

    for (const auto& m : movies)
        ids.insert(m.id);

    EXPECT_EQ(ids, (std::set<MovieId>{"m1", "m2", "m3"}));
}

TEST(ListMovies, SortedById) {
    BookingService svc;
    auto movies = svc.listMovies();

    test_movies = {
        {"m1", "The Matrix"},
        {"m2", "Inception"},
        {"m3", "Interstellar"}
    };

    EXPECT_EQ(test_movies, movies);
}

TEST(ListTheaters, KnownMovie) {
    BookingService svc;
    auto theater = svc.listTheatersForMovie("m1");
    EXPECT_EQ(theater.size(), 2);
}

TEST(ListTheaters, UnknownMovie) {
    BookingService svc;
    EXPECT_TRUE(svc.listTheatersForMovie("DOES_NOT_EXIST").empty());
}

TEST(ListSeats, AllAvailableInitially) {
    BookingService svc;
    auto seats = svc.listAvailableSeats("m1", "t1");
    EXPECT_EQ(seats.size(), 20);
}

TEST(ListSeats, UnknownShowing) {
    BookingService svc;
    EXPECT_TRUE(svc.listAvailableSeats("m1", "t3").empty()); 
    EXPECT_TRUE(svc.listAvailableSeats("NOPE",  "t1").empty());
}

TEST(Book, SingleSeat) {
    BookingService svc;
    auto r = svc.bookSeats("m1", "t1", {"a1"});
    ASSERT_TRUE(r.success);
    EXPECT_EQ(r.bookedSeats, std::vector<SeatId>{"a1"});
    EXPECT_FALSE(contains(svc.listAvailableSeats("m1", "t1"), "a1"));
}

TEST(Book, MultipleSeats) {
    BookingService svc;
    auto r = svc.bookSeats("m1", "t1", {"a1", "a2", "a3"});
    ASSERT_TRUE(r.success);
    EXPECT_EQ(r.bookedSeats.size(), 3);
}

TEST(Book, EmptyRequestFails) {
    BookingService svc;
    EXPECT_FALSE(svc.bookSeats("m1", "t1", {}).success);
}

TEST(Book, UnknownShowingFails) {
    BookingService svc;
    EXPECT_FALSE(svc.bookSeats("m1", "t3", {"a1"}).success);
}

TEST(Book, InvalidSeatIdFails) {
    BookingService svc;
    EXPECT_FALSE(svc.bookSeats("m1", "t1", {"a99"}).success);
    
    EXPECT_EQ(svc.listAvailableSeats("m1", "t1").size(),20);
}

TEST(Book, DuplicatesInRequestFail) {
    BookingService svc;
    EXPECT_FALSE(svc.bookSeats("m1", "t1", {"a1", "a2", "a1"}).success);
}

TEST(Book, AlreadyBookedFails) {
    BookingService svc;
    ASSERT_TRUE(svc.bookSeats("m1", "t1", {"a1"}).success);
    EXPECT_FALSE(svc.bookSeats("m1", "t1", {"a1"}).success);
}

// The key correctness property: a conflict in any one seat must roll back
// the whole request.
TEST(Book, AllOrNothing) {
    BookingService svc;
    ASSERT_TRUE(svc.bookSeats("m1", "t1", {"a2"}).success);

    auto res = svc.bookSeats("m1", "t1", {"a1", "a2", "a3"});
    EXPECT_FALSE(res.success);

    auto avail = svc.listAvailableSeats("m1", "t1");
    EXPECT_TRUE(contains(avail, "a1"));
    EXPECT_FALSE(contains(avail, "a2"));
    EXPECT_TRUE(contains(avail, "a3"));
}

TEST(Book, ShowingsAreIsolated) {
    BookingService svc;
    ASSERT_TRUE(svc.bookSeats("m1", "t1", {"a1"}).success);
    // Same seat id in a different showing must still be free.
    EXPECT_TRUE(contains(svc.listAvailableSeats("m1", "t2"), "a1"));
}
