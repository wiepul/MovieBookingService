#include <iostream>
#include "BookingService.h"

void printMenu() {
    std::cout <<
        "\n=== Movie Booking ===\n"
        " 1. List movies\n"
        " 2. List theaters showing a movie\n"
        " 3. List available seats\n"
        " 4. Book seats\n"
        " 5. Exit\n";
}

int main() {
    BookingService svc;
    std::cout << "Movie Booking CLI. Pick an option from the menu.\n";
    while (true)
    {
        printMenu();
        std::cout << "> ";
        std::string choice;
        if (!std::getline(std::cin, choice)) break;

        if (choice == "1") {
            auto movies = svc.listMovies();
            std::cout << "Movies:\n";
            for (const auto& m : movies) {
                std::cout << " - " << m.id << ": " << m.title << "\n";
            }
        } else if (choice == "2") {
            std::cout << "Enter movie ID: ";
            std::string movieId;
            if (!std::getline(std::cin, movieId)) break;
            auto theaters = svc.listTheatersForMovie(movieId);
            std::cout << "Theaters showing movie " << movieId << ":\n";
            for (const auto& t : theaters) {
                std::cout << " - " << t.id << ": " << t.name << "\n";
            }
        } else if (choice == "3") {
            std::cout << "Feature not implemented yet.\n";
        } else if (choice == "4") {
            std::cout << "Feature not implemented yet.\n";
        } else if (choice == "5") {
            std::cout << "Goodbye!\n";
            break;
        } else {
            std::cout << "Invalid choice. Please try again.\n";
        }
    }
    
    return 0;


}
