#include <iostream>
#include "BookingService.h"

void printMenu()
{
    std::cout << "\n=== Movie Booking ===\n"
                 " 1. List movies\n"
                 " 2. List theaters showing a movie\n"
                 " 3. List available seats\n"
                 " 4. Book seats\n"
                 " 5. Exit\n";
}

std::string prompt(const std::string &label)
{
    std::cout << label;
    std::string line;
    if (!std::getline(std::cin, line))
        return {};

    return line;
}

// This only handles commas and whitespace;
std::vector<std::string> parseSeats(const std::string &s)
{
    std::vector<std::string> out;
    std::string cur;
    for (char c : s)
    {
        if (c == ',' || std::isspace(static_cast<unsigned char>(c)))
        {
            if (!cur.empty())
            {
                out.push_back(cur);
                cur.clear();
            }
        }
        else
        {
            cur.push_back(c);
        }
    }
    if (!cur.empty())
        out.push_back(cur);
    return out;
}

int main()
{
    BookingService svc;
    std::cout << "Movie Booking CLI. Pick an option from the menu.\n";
    while (true)
    {
        printMenu();
        std::cout << "> ";
        std::string choice;
        if (!std::getline(std::cin, choice))
            break;

        if (choice == "1")
        {
            auto movies = svc.listMovies();

            if (movies.empty())
            {
                std::cout << "No movies.\n";
                return 0;
            }

            std::cout << "Movies:\n";

            for (const auto &m : movies)
            {
                std::cout << " - " << m.id << ": " << m.title << "\n";
            }
        }
        else if (choice == "2")
        {

            auto movieId = prompt("Enter movie ID: ");

            auto theaters = svc.listTheatersForMovie(movieId);

            if (theaters.empty())
            {
                std::cout << "No theaters for '" << movieId << "'.\n";
                return 0;
            }

            std::cout << "Theaters showing movie " << movieId << ":\n";

            for (const auto &t : theaters)
            {
                std::cout << " - " << t.id << ": " << t.name << "\n";
            }
        }
        else if (choice == "3")
        {
            auto movieId = prompt("Enter movie ID: ");

            auto theaterId = prompt("Enter Theater ID: ");

            const auto seats = svc.listAvailableSeats(movieId, theaterId);

            if (seats.empty())
            {
                std::cout << "No available seats (showing may not exist).\n";
                return 0;
            }

            std::cout << "Available (" << seats.size() << "):";

            for (const auto &s : seats)
                std::cout << ' ' << s;

            std::cout << '\n';
        }
        else if (choice == "4")
        {

            const auto movieId = prompt("Movie id: ");
            const auto theaterId = prompt("Theater id: ");
            const auto line = prompt("Seats (e.g. a1,a2): ");

            const auto seats = parseSeats(line);

            if (seats.empty())
            {
                std::cout << "No seats given.\n";
                return 0;
            }

            const auto r = svc.bookSeats(movieId, theaterId, seats);

            if (r.success)
            {
                std::cout << "Booked:";

                for (const auto &s : r.bookedSeats)
                    std::cout << ' ' << s;

                std::cout << '\n';
            }
            else
            {
                std::cout << "Failed: " << r.message << '\n';
            }
        }
        else if (choice == "5")
        {
            std::cout << "Goodbye!\n";
            break;
        }
        else
        {
            std::cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
