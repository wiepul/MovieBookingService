
#include <string>
#include <vector>

using namespace std;

struct Movie {
    string     id;
    std::string title;
};

struct Theater {
    string   id;
    std::string name;
};

class BookingService {
public:
    BookingService();
    std::vector<Movie> listMovies() const;
    std::vector<Theater> listTheatersForMovie(std::string movieId) const;
    void bookSeats(std::string movieId,
                            std::string theaterId,
                            const std::vector<string>& seats);

private:
    std::vector<Movie> movies;
    std::vector<Theater> theaters;
};
